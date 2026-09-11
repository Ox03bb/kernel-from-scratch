#include "kernel.h"

#include "idt.h"

#include "pic.h"
#include "pit.h"
#include "timer.h"

#include "vga.h"
#include "vga_lib.h"

#include "panic.h"

#include "utils.h"

void kernel_main() {

    vga_init();
    vga_log_info("[", "init", "] vga");
    vga_print_at_end_c("... Ok\n", GREEN);

    vga_log_info("[", "init", "] pic");
    pic_init();
    vga_print_at_end_c("... Ok\n", GREEN);

    vga_log_info("[", "init", "] idt");
    idt_init();
    vga_print_at_end_c("... Ok\n", GREEN);

    vga_log_info("[", "init", "] pit and timer");
    pic_clear_mask(0);
    timer_init(1000); // 1000 Hz
    
    sti();

    for (int i = 0; i < 53; i++) {
        vga_print(".");
        timer_sleep(0.02);
    }
    vga_print_at_end_c("... Ok\n", GREEN);  // timer 

    for (;;) {
        asm volatile("hlt");
    }

    return;
}
