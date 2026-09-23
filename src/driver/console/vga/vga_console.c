#include "driver/vga.h"

#include "types.h"

#include "console.h"s

#include "driver/tty.h"

#include "stdio.h"

void console_write(console_t *console, char *str);
void console_clear(console_t *console);


static const console_interface_t console_default_interface = {
    .write = console_write,
    .clear = console_clear,
};


void console_init(console_t *console) {
    if (console == NULL)
        return;

    console->method = console_default_interface;
}

void console_write(console_t *console, char *str) {

    if (console == NULL || str == NULL)
        return;

    print(str);
}

void console_clear(console_t *console) {
    if (console == NULL)
        return;

    vga_clear();
}