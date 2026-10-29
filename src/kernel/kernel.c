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
#include "mm/vmm.h"

#include "keyboard/event_queue.h"
#include "keyboard/handler.h"

uint8_t pmm_bitmap[BITMAP_SIZE];

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

    pmm_memory_map(&memory_map);
    pmm_bitmap[0] |= 1U; // reserve the address 0x0 to NULL
    KERNEL_INIT_P("PPM - Physical Memory Manager");

    /*
     * Keep BIOS and bootloader data, including the temporary stack and E820
     * buffer, out of the physical frame allocator.
     */
    pmm_reserve(0, (uintptr_t)&kernel_start_addr);
    pmm_reserve((uintptr_t)&kernel_start_addr, (uintptr_t)&kernel_end_addr);
    KERNEL_INIT_P("Reserved kernel memory");

    // uintptr_t ptr = pmm_alloc_n_frame(100);
    // printf("%x\n", ptr);

    // bool c = pmm_check_mm(ptr);

    // printf("%b\n", c);

    // tty_t tty0;
    // tty_init(&tty0);

    KERNEL_INIT("Virtual Memory & Paging ", vmm_init);

    static volatile int *a = (volatile int *)0xfffffff;

    *a = 4;
    
    while (1) {
        // tty_process_events(&tty0);
    }

    print("[\033[02:00mready\033[0m] Kernel initialized successfully!\n");
    return;
}