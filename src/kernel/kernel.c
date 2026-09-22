#include "kernel.h"

#include "idt.h"

#include "pic.h"
#include "pit.h"
#include "ps2.h"
#include "timer.h"

#include "keyboard.h"
#include "vga.h"

#include "vga_lib.h"

#include "panic.h"

#include "stdio.h"
#include "utils.h"

#include "string.h"

#include "keyboard/event_queue.h"
#include "keyboard/handler.h"

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
    KERNEL_INIT("Keyboard Driver", init_keyboard);

    pic_clear_mask(1);

    while (1) {
        input_event_t event;
        if (!input_queue_pop(&event)) {
            asm volatile("hlt");
            continue;
        }

        if (event.type == INPUT_EVENT_CHAR) {
            print("Character: ");
            print_char(event.character);
            print("\n");
            print("action:  ");
            print(event.action == KEY_PRESSED ? "pressed\n" : "released\n");
        } else if (event.key == KEY_ENTER && event.action == KEY_PRESSED) {
            print("Enter key pressed\n");
        } else if (event.key == KEY_BACKSPACE && event.action == KEY_PRESSED) {
            print("Backspace key pressed\n");
        } else if (event.key == KEY_TAB && event.action == KEY_PRESSED) {
            print("Tab key pressed\n");
        } else if (event.key == KEY_UP && event.action == KEY_PRESSED) {
            print("Up arrow key pressed\n");
        } else if (event.key == KEY_DOWN && event.action == KEY_PRESSED) {
            print("Down arrow key pressed\n");
        } else if (event.key == KEY_LEFT && event.action == KEY_PRESSED) {
            print("Left arrow key pressed\n");
        } else if (event.key == KEY_RIGHT && event.action == KEY_PRESSED) {
            print("Right arrow key pressed\n");
        } else if (event.key == KEY_LEFT_CTRL) {
            print("Ctrl key ");
            print(event.action == KEY_PRESSED ? "pressed\n" : "released\n");
        }
    }

    return;
}