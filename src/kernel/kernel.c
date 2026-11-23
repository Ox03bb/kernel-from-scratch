#include "kernel.h"

#include "idt.h"

#include "pic.h"
#include "ps2.h"

#include "utils/kernal_utils.h"
#include "utils/utils.h"

#include "keyboard.h"
#include "vga.h"

#include "panic.h"

#include "stdio.h"
#include "string.h"

#include "driver/tty.h"

#include "fs.h"
#include "shell/shell.h"

#include "mm/pmm.h"
#include "mm/vmm.h"

#include "mm/heap.h"

#include "sched.h"
#include "thread.h"

#include "keyboard/event_queue.h"
#include "keyboard/handler.h"

#include "timer.h"

uint8_t pmm_bitmap[BITMAP_SIZE];

heap_t k_heap;

scheduler_t *sched;

static shell_context_t shell;

extern uintptr_t kernel_start_addr;
extern uintptr_t kernel_end_addr;

// heap_t kheap;

static void shell_thread_entry(void) {
    while (1)
        shell_process(&shell);
}

void t1(int number, int count, int delay) {
    for (int i = 0; i < 10; i++) {
        printf("thread 1 \n", i);
    }
}
void t2(void) {
    int i = 0;
    while (i < 10) {
        printf("thread 2 \n ");
        i++;
    }
}

void kernel_main(boot_info_t *boot_info) {

    if (boot_info == NULL) {
        panic("Boot info is NULL");
    }

    vga_init();
    KERNEL_INIT_P("VGA Console");

    KERNEL_INIT("PIC - Programmable Interrupt Controller", pic_init);

    KERNEL_INIT("IDT - Interrupt Descriptor Table", idt_init);

    KERNEL_INIT("PS/2 controller", ps2_init);
    KERNEL_INIT("Keyboard Driver", init_keyboard);
    pic_clear_mask(1);

    memory_map_t memory_map;
    pmm_memory_detect(&memory_map, boot_info, true);
    memset(pmm_bitmap, 0xFF, sizeof(pmm_bitmap));

    pmm_memory_map(&memory_map);
    pmm_bitmap[0] |= 1U; // reserve the address 0x0 to NULL
    KERNEL_INIT_P("PPM - Physical Memory Manager");

    pmm_reserve(0, (uintptr_t)&kernel_start_addr);
    pmm_reserve((uintptr_t)&kernel_start_addr, (uintptr_t)&kernel_end_addr);
    KERNEL_INIT_P("Reserved kernel memory");

    KERNEL_INIT("VMM - Virtual Memory & Paging ", vmm_init);

    current_heap = &k_heap;
    init_heap(current_directory, current_heap);
    KERNEL_INIT_P("Kernal Heap");

    if (fs_init() < 0)
        panic("Filesystem initialization failed");
    KERNEL_INIT_P("Filesystem");

    KERNEL_INIT("Scheduler", sched_init);

    KERNEL_INIT("PIT - Programmable Interval Timer", timer_setup); // 1000 Hz

    tty_t tty0;
    tty_init(&tty0);
    KERNEL_INIT_P("TTY - Teletypewriter 0");

    KERNEL_INIT_P("Shell");

    vga_clear();

    if (shell_init(&shell, &tty0) < 0)
        panic("Shell initialization failed");

    // thread_t *t01 = thread_spawn(t1, 5, 10, 1);
    // thread_t *t02 = thread_spawn(t2);

    thread_t *t01 = thread_spawn(shell_thread_entry);
    if (t01 == NULL)
        panic("Shell thread creation failed");

    sched_start();

    while (1) {
        // hlt();
    }

    // print("[\033[02:00mready\033[0m] Kernel initialized successfully!\n");
    return;
}