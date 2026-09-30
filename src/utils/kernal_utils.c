#include "kernal_utils.h"

#include "pic.h"
#include "stdio.h"
#include "timer.h"
#include "utils.h"
#include "vga.h"
#include "vga_lib.h"

#define TIMER_FREQ 1000

void KERNEL_INIT(const char *name, void (*init_function)(void)) {
    print("[\033[34minit\033[0m] ");
    vga_print(name);

    init_function();

    print_at_end("... Ok\n", GREEN);
}

void KERNEL_INIT_P(const char *name) {
    print("[\033[34minit\033[0m] ");
    vga_print(name);

    print_at_end("... Ok\n", GREEN);
}

void timer_setup(void) {
    pic_clear_mask(0);
    timer_init(TIMER_FREQ);

    sti();

    for (int i = 0; i < 33; i++) {
        vga_print(".");
        timer_sleep(0.02);
    }
}
