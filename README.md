[![Assembly](https://img.shields.io/badge/x86%20Assembly-6e4c13?style=flat&logo=assembly&logoColor=white)](https://www.nasm.us/)
[![C](https://img.shields.io/badge/-C/C+-142282?style=flat&logo=C&logoColor=white)](https://en.cppreference.com/w/c)
[![QEMU](https://img.shields.io/badge/-QEMU-FF6600?style=flat&logo=qemu&logoColor=white)](https://www.qemu.org/)
[![GDB](https://img.shields.io/badge/-GDB-4EAA25?style=flat&logo=gnu&logoColor=white)](https://www.gnu.org/software/gdb/)

# Kernel From Scratch

An educational x86 operating system kernel built from scratch using C and Assembly. The project explores operating system internals, low-level programming, memory management, interrupt handling, multitasking, and hardware interaction.


## Overview

Kernel From Scratch is an educational operating system project focused on understanding how operating systems work at a low level.

Rather than relying on an existing kernel or operating system framework, this project implements fundamental kernel components, including a custom BIOS bootloader, interrupt management, physical and virtual memory management, kernel heap allocation, thread scheduling, hardware drivers, and an in-memory filesystem.

The kernel also provides an interactive command-line environment, allowing users to interact with its filesystem and built-in system utilities.

The project is developed primarily in C and x86 Assembly and is designed to run in QEMU.

## Features

### Bootloader and CPU Initialization
- Custom BIOS bootloader.
- Kernel loading from disk.
- Transition from real mode to 32-bit protected mode.
- Global Descriptor Table (GDT) initialization.
- Kernel entry point and hardware information initialization.

### Interrupt Management
- Interrupt Descriptor Table (IDT) with 256 entries.
- Interrupt Service Routines (ISR).
- Interrupt Request (IRQ) handling.
- Programmable Interrupt Controller (PIC) remapping.
- CPU exception handling.

### Memory Management
- E820 memory map detection.
- Physical Memory Manager (PMM) using a bitmap allocator.
- Physical frame allocation and deallocation.
- Virtual Memory Manager (VMM).
- Page directory and page table management.
- Virtual address mapping and unmapping.
- Kernel heap with dynamic memory allocation.

### Multitasking and Thread Management
- Thread creation and initialization.
- Independent thread stacks.
- CPU context management and context switching.
- Thread states and ready queue.
- Bootstrap and idle threads.
- Preemptive scheduling using the PIT timer.

### Hardware Drivers
- VGA text-mode console.
- PS/2 keyboard driver.
- Programmable Interval Timer (PIT).
- Real-Time Clock (RTC).

### Console and TTY
- Keyboard event processing.
- Input buffering and line editing.
- Cursor navigation.
- Command history.
- Keyboard modifier handling.
- Ctrl+C interrupt request.

### RAMFS — In-Memory Filesystem
- Hierarchical filesystem structure.
- File and directory nodes.
- Parent-child relationships.
- Path resolution and lookup.
- File creation and deletion.
- File reading and writing.
- Directory operations.

### Interactive Shell
- Command registration and execution.
- Command-line argument parsing.
- Environment variables.
- Command history.
- Filesystem navigation and manipulation.
- Built-in system utilities.

### Built-in Commands

| Command | Description |
|---|---|
| `help` | Display available commands |
| `history` | Display command history |
| `clear` | Clear the terminal |
| `echo` | Print text |
| `pwd` | Display the current directory |
| `ls` | List directory contents |
| `cd` | Change the current directory |
| `mkdir` | Create a directory |
| `touch` | Create a file |
| `rm` | Remove a file or directory |
| `cat` | Display file contents |
| `tree` | Display the directory hierarchy |
| `ed` | Edit file contents |
| `cp` | Copy files |
| `mv` | Move or rename files |
| `sleep` | Suspend execution for a specified duration |
| `info` | Display kernel information |
| `reboot` | Restart the system |
| `shutdown` | Halt the system |

## Project Structure

```txt
.
├── conf.gdb
├── Dockerfile
├── linker.ld
├── Makefile
├── README.md
├── bin/
├── build/
├── docs/
├── includes/
│   ├── kernel.h
│   ├── panic.h
│   ├── timer.h
│   ├── driver/
│   ├── interrupt/
│   ├── fs/
│   ├── lib/
│   ├── mm/
│   ├── sched/
│   ├── shell/
│   └── utils/
├── scripts/
│   └── requirements.sh
├── src/
│   ├── driver/
│   ├── kernel/ # Core kernel components 
│   │   ├── fs/
│   │   ├── sched/
│   │   ├── shell/
│   │   └── ...
│   ├── arch/ # Architecture-specific implementation
│   │   └── x86/
│   │       ├── bootloader/
│   │       ├── driver/
│   │       ├── interrupt/
│   │       ├── kernel/
│   │       ├── mm/
│   │       └── utils/
│   ├── libs/
│   └── utils/
└── test/
```

## Requirements

The project uses Docker to provide a consistent build environment.

Required tools:

- [Git](https://git-scm.com/) — Source code management.
- [Docker](https://www.docker.com/) — Build environment.
- [QEMU](https://www.qemu.org/) — x86 system emulator.
- [Make](https://www.gnu.org/software/make/) — Build automation.
- [NASM](https://www.nasm.us/) — x86 assembler.

The Docker build environment provides the cross-compilation toolchain, including `i686-elf-gcc`, `i686-elf-ld`, and `i686-elf-objcopy`.

For debugging, [GDB](https://www.gnu.org/software/gdb/) is also recommended. [Pwndbg](https://github.com/pwndbg/pwndbg) can be used as an optional GDB enhancement.


### Operating System

Linux is recommended for development.

- **If you're using Linux:**
  Follow the installation and setup instructions below.

- **If you're using `SpywareOS` (also known as `Windows`):**
  1. Uninstall it.
  2. Install Linux.
  3. Run the commands below.
>**Note:** you should do this even u dont want to run this project, Trust me broo (;

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/Ox03bb/kernel-from-scratch.git
cd kernel-from-scratch
```

### 2. Prepare the Build Environment

Build the Docker image and compile the kernel:

```bash
make setup
```

The setup target builds the Docker image if it does not already exist, then invokes the default build target.

### 3. Build the Kernel

Once the Docker image is available:

```bash
make
```

The resulting bootable disk image is generated at:

```text
bin/os.bin
```

### 4. Run the Kernel

Launch the kernel in QEMU:

```bash
make run
```

### 5. Debug the Kernel

Launch QEMU with its GDB debugging server enabled:

```bash
make debug
```

QEMU starts paused and exposes its GDB server on the default port `1234`.

Connect using:

```bash
gdb
```

Then, inside GDB:

```gdb
target remote :1234
symbol-file build/kernel.elf
```

## Development

The Makefile provides additional development utilities:

| Target | Description |
|---|---|
| `make` | Build the kernel |
| `make setup` | Prepare Docker environment and build |
| `make run` | Run the kernel in QEMU |
| `make debug` | Run QEMU with GDB debugging support |
| `make clean` | Remove generated build artifacts |
| `make format` | Format C source and header files |
| `make format-check` | Check source formatting |
| `make lint` | Run Clang-Tidy |

## Roadmap

Planned directions for future development include:

- Expanding kernel system interfaces.
- Improving thread synchronization and scheduling.
- Adding more filesystem capabilities.
- Developing additional hardware drivers.
- Implementing a framebuffer graphics subsystem.
- Exploring software-based 3D rendering.
- Improving kernel diagnostics and debugging tools.

