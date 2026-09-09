#include "kernel.h"
#include "idt.h"
#include "panic.h"
#include "pic.h"
#include "pit.h"
#include "utils.h"
#include "vga.h"
#include "vga_lib.h"

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

    vga_log_info("[", "init", "] pit");
    pic_clear_mask(0);
    pit_init(100); // 100 Hz
    vga_print_at_end_c("... Ok\n", GREEN);

    // asm volatile("int $0x22");

    // panic("testing panics");

    asm volatile("sti");

    for (;;) {
        asm volatile("hlt");
    }

    return;
}
