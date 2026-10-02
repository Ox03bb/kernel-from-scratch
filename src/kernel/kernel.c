#include "kernel.h"

#include "idt.h"

#include "pic.h"
#include "ps2.h"

#include "utils/kernal_utils.h"

#include "keyboard.h"
#include "vga.h"

#include "panic.h"

#include "stdio.h"
#include "string.h"

#include "driver/tty.h"

#include "mm/pmm.h"

#include "keyboard/event_queue.h"
#include "keyboard/handler.h"

static uint8_t pmm_bitmap[BITMAP_SIZE];
extern uintptr_t kernel_start_addr;
extern uintptr_t kernel_end_addr;

void kernel_main(boot_info_t *boot_info) {

    if (boot_info == NULL) {
        panic("Boot info is NULL");
    }

    KERNEL_INIT("VGA console", vga_init);

    KERNEL_INIT("PIC - Programmable Interrupt Controller", pic_init);

    KERNEL_INIT("IDT - Interrupt Descriptor Table", idt_init);

    KERNEL_INIT("PIT - Programmable Interval Timer", timer_setup); // 1000 Hz

    KERNEL_INIT("PS/2 controller", ps2_init);
    KERNEL_INIT("Keyboard Driver", init_keyboard);
    pic_clear_mask(1);

    memory_map_t memory_map;
    pmm_memory_detect(&memory_map, boot_info, true);
    memset(pmm_bitmap, 0xFF, sizeof(pmm_bitmap));

    pmm_memory_map(&memory_map, pmm_bitmap);
    KERNEL_INIT_P("PPM - Physical Memory Manager");

    pmm_reserve(pmm_bitmap, (uintptr_t)&kernel_start_addr, (uintptr_t)&kernel_end_addr);
    KERNEL_INIT_P("Reserved kernel memory");

    uintptr_t ptr = pmm_alloc_n_frame(pmm_bitmap, 100);
    printf("%x\n", ptr);
    // ptr -= 0x1000;
    bool c = pmm_check_mm(pmm_bitmap, ptr);

    printf("%bl", c);

    tty_t tty0;
    tty_init(&tty0);

    while (1) {

        // tty_process_events(&tty0);
    }

    print("[\033[32mready\033[0m] Kernel initialized successfully!\n");
    return;
}