#ifndef TTY_H
#define TTY_H

#include "keyboard_keys.h"
#include "keyboard_layout.h"
#include "types.h"
#include "driver/keyboard/event_queue.h"

#define TTY_BUFFER_SIZE 4096

// modifiers
typedef enum {
    MOD_NONE   = 0,
    MOD_SHIFT  = 1 << 0,
    MOD_CTRL   = 1 << 1,
    MOD_ALT    = 1 << 2,
    MOD_ALTGR  = 1 << 3,
    MOD_CAPS   = 1 << 4,
    MOD_NUM    = 1 << 5,
    MOD_SCROLL = 1 << 6,
    MOD_SUPER  = 1 << 7
} keyboard_modifiers_t;

struct tty_t;

typedef struct {
    void (*init)(struct tty_t *tty);
    void (*putc)(struct tty_t *tty, char c);
    void (*write)(struct tty_t *tty, const char *str);
    void (*read)(struct tty_t *tty, char *buf, size_t len);
    void (*clear)(struct tty_t *tty);
} tty_interface_t;

typedef struct tty_t {
    bool echo;
    bool line_ready;

    int input_length;
    int buffer_index;
    char buffer[TTY_BUFFER_SIZE];

    keyboard_modifiers_t modifiers;

    tty_interface_t method;
} tty_t;

void tty_handle_event(tty_t *tty, const input_event_t *event);
void tty_update_modifiers(tty_t *tty, const input_event_t *event);

#endif