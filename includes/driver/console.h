#ifndef CONSOLE_H
#define CONSOLE_H

#include "driver/keyboard/event_queue.h"
#include "keyboard_keys.h"
#include "keyboard_layout.h"
#include "types.h"

struct console_t;

typedef struct {
    void (*write)(struct console_t *console, char *str);
    void (*clear)(struct console_t *console);
} console_interface_t;

typedef struct console_t {
    console_interface_t method;
} console_t;

void console_init(console_t *console);


#endif