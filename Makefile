SRC_DIR := src
INCLUDE_DIR := includes
BUILD_DIR := build
BIN_DIR := bin

KERNEL_ASM_SRCS := $(shell find $(SRC_DIR) -type f \
	\( -name '*.asm' -o -name '*.s' \) \
	! -path '$(SRC_DIR)/bootloader/*' \
	! -path '$(SRC_DIR)/mm/physical/pmm.asm')
KERNEL_C_SRCS := $(shell find $(SRC_DIR) -name '*.c')

BOOT_ASM := $(SRC_DIR)/bootloader/boot.asm

OS_BIN := $(BIN_DIR)/os.bin

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BIN_DIR)/kernel.bin
KERNEL_SECTORS := $(BUILD_DIR)/kernel_sectors.inc

LINKER_SCRIPT := linker.ld

ASM := nasm
CC := i686-elf-gcc
LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy

CLANG_FORMAT := clang-format
CLANG_TIDY := clang-tidy

DOCKER_IMAGE := cc

DOCKER_RUN := docker run --rm \
	--user $(shell id -u):$(shell id -g) \
	-v $(CURDIR):/work \
	-w /work \
	-e PATH=/home/linuxbrew/.linuxbrew/bin:$$PATH \
	$(DOCKER_IMAGE)

INCLUDE_DIRS := $(shell find $(INCLUDE_DIR) -type d)

CFLAGS := \
	-g \
	-O0 \
	-Wall \
	-std=gnu99 \
	-ffreestanding \
	-nostdlib \
	-nostartfiles \
	-nodefaultlibs \
	$(foreach dir,$(INCLUDE_DIRS),-I$(dir))
	
.PHONY: all clean run debug format format-check lint

all: $(OS_BIN)


$(OS_BIN): $(BOOT_ASM) $(KERNEL_ASM_SRCS) $(KERNEL_C_SRCS) $(LINKER_SCRIPT)
	@echo "Building kernel inside Docker..."
	@mkdir -p $(BIN_DIR)
	$(DOCKER_RUN) /bin/bash -c '\
		set -e; \
		\
		echo "== Compiling kernel assembly =="; \
		$(foreach src,$(KERNEL_ASM_SRCS),mkdir -p $(dir build/$(subst src/,,$(src:.asm=.asm.o))) && nasm -f elf32 -g $(src) -o build/$(subst src/,,$(src:.asm=.asm.o)) &&) \
		\
		echo "== Compiling kernel C =="; \
		$(foreach src,$(KERNEL_C_SRCS),mkdir -p $(dir build/$(subst src/,,$(src:.c=.o))) && i686-elf-gcc $(CFLAGS) -c $(src) -o build/$(subst src/,,$(src:.c=.o)) &&) \
		\
		echo "== Combining objects =="; \
		i686-elf-ld -r \
			-o build/kernel-linked.o \
			$$(find build -name "*.o" ! -name "kernel-linked.o") && \
		\
		echo "== Linking ELF kernel =="; \
		i686-elf-gcc $(CFLAGS) \
			-T linker.ld \
			-o build/kernel.elf \
			build/kernel-linked.o && \
		\
		echo "== Creating raw kernel binary =="; \
		i686-elf-objcopy -O binary \
			build/kernel.elf \
			bin/kernel.bin && \
		\
		KERNEL_SIZE=$$(stat -c%s bin/kernel.bin); \
		KERNEL_SECTORS=$$(( (KERNEL_SIZE + 511) / 512 )); \
		\
		echo "Kernel size: $$KERNEL_SIZE bytes"; \
		echo "Kernel sectors: $$KERNEL_SECTORS"; \
		\
		truncate -s $$((KERNEL_SECTORS * 512)) bin/kernel.bin && \
		\
		echo "KERNEL_SECTORS equ $$KERNEL_SECTORS" \
			> build/kernel_sectors.inc && \
		\
		echo "== Assembling bootloader =="; \
		nasm -f bin \
			src/bootloader/boot.asm \
			-o bin/boot.bin && \
		\
		echo "== Creating OS image =="; \
		cat bin/boot.bin > bin/os.bin && \
		cat bin/kernel.bin >> bin/os.bin && \
		\
		echo "OS image created successfully" \
	'


run: $(OS_BIN)
	qemu-system-i386 \
		-drive file=$(OS_BIN),format=raw \
		-m 4G \
		-serial stdio

crun: $(OS_BIN)
	make clean
	make run

setup:
	@if ! docker image inspect $(DOCKER_IMAGE):latest >/dev/null 2>&1; then \
		docker build -t $(DOCKER_IMAGE):latest .; \
	fi
	$(MAKE)


debug: $(OS_BIN)
	qemu-system-i386 \
		-drive file=$(OS_BIN),format=raw \
		-m 4G \
		-serial stdio \
		-S -s


format:
	find $(SRC_DIR) $(INCLUDE_DIR) -type f \
		\( -name '*.c' -o -name '*.h' \) \
		-exec $(CLANG_FORMAT) -i {} +


format-check:
	find $(SRC_DIR) $(INCLUDE_DIR) -type f \
		\( -name '*.c' -o -name '*.h' \) \
		-exec $(CLANG_FORMAT) --dry-run --Werror {} +


lint:
	find $(SRC_DIR) -type f -name '*.c' \
		-exec $(CLANG_TIDY) {} -- \
		-I$(INCLUDE_DIR) \
		-std=gnu99 \
		-ffreestanding \
		-target i386-elf \;


clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)