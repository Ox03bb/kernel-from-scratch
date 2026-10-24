#include "panic.h"
#include "utils/utils.h"
#include "vga.h"
#include "vga_lib.h"

void panic(char *err) {
    vga_print("\n");
    vga_log_error("", "\nKERNEL PANIC: ", err);

    cli();

    for (;;)
        hlt();
}

void exception(char *err) {
    vga_print("\n");
    vga_log_error("", "\nKERNEL EXCEPTION: ", err);

    cli();

    for (;;)
        hlt();
}