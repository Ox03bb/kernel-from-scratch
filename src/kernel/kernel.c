#include "kernel.h"

#include "idt.h"

#include "pic.h"
#include "pit.h"
#include "ps2.h"
#include "timer.h"

#include "vga.h"
#include "keyboard.h"

#include "vga_lib.h"

#include "panic.h"

#include "stdio.h"
#include "utils.h"

#define TIMER_FREQ 1000 // 1000 Hz

void KERNEL_INIT(const char *name, void (*init_function)(void)) {
    print("[\033[34minit\033[0m] ");
    vga_print(name);

    init_function();

    print_at_end("... Ok\n", GREEN);
}

void timer_setup() {
    pic_clear_mask(0);
    timer_init(TIMER_FREQ); // 1000 Hz

    sti();

    for (int i = 0; i < 33; i++) {
        vga_print(".");
        timer_sleep(0.02);
    }
}

void kernel_main() {

    KERNEL_INIT("VGA console", vga_init);

    KERNEL_INIT("PIC - Programmable Interrupt Controller", pic_init);

    KERNEL_INIT("IDT - Interrupt Descriptor Table", idt_init);

    KERNEL_INIT("PIT - Programmable Interval Timer", timer_setup); // 1000 Hz

    KERNEL_INIT("PS/2 controller", ps2_init);

    if (ps2_output_buffer_full()) {
    uint8_t data = ps2_read();
    printf("SCAN: %x\n", data);
    }

    printf("====================================\n");
    KERNEL_INIT("Keyboard Driver", init_keyboard);
    while (1){
        if (ps2_output_buffer_full()) {
        uint8_t data = ps2_read();
        printf("SCAN: %d\n", data);
        }
    }
    
   


    for (;;) {
        asm volatile("hlt");
    }

    return;
}