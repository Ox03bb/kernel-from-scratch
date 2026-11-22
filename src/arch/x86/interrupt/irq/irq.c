#include "irq.h"
#include "context.h"
#include "keyboard.h"
#include "keyboard/handler.h"
#include "pic.h"
#include "sched.h"
#include "timer.h"
#include "vga.h"
#include "vga_lib.h"

interrupt_frame_t *active_irq_frame;
extern scheduler_t *sched;

void irq_handler(uint32_t vector) {

    switch (vector) {
    case 0:
        timer_irq_handler();
        pic_send_eoi((uint8_t)vector);
        if (sched->need_resched) {
            sched->need_resched = false;
            sched->remaining_ticks = sched->time_quantum;
            sched_schedule();
        }
        break;

    case 1:
        keyboard_irq_handler();
        pic_send_eoi((uint8_t)vector);
        break;

    default:
        vga_print("IRQ: ");
        vga_print_hex(vector);
        vga_print("\n");

        pic_send_eoi((uint8_t)vector);
    }
}