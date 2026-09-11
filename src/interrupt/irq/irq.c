#include "irq.h"
#include "pic.h"
#include "timer.h"
#include "vga.h"
#include "vga_lib.h"

void irq_handler(uint32_t vector) {

    switch (vector) {
    case 0:
        timer_irq_handler();
        pic_send_eoi((uint8_t)vector);
        break;

    default:
        vga_print("IRQ: ");
        vga_print_hex(vector);
        vga_print("\n");

        pic_send_eoi((uint8_t)vector);
    }
}