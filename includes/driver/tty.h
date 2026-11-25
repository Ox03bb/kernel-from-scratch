#ifndef TTY_H
#define TTY_H

#include "driver/keyboard/event_queue.h"
#include "keyboard_keys.h"
#include "keyboard_layout.h"
#include "types.h"

#include "console.h"

#define TTY_BUFFER_SIZE        4096
#define TTY_HISTORY_LINE_SIZE  256

// modifiers
typedef enum {
    MOD_NONE = 0,
    MOD_SHIFT = 1 << 0,
    MOD_CTRL = 1 << 1,
    MOD_ALT = 1 << 2,
    MOD_ALTGR = 1 << 3,
    MOD_CAPS = 1 << 4,
    MOD_NUM = 1 << 5,
    MOD_SCROLL = 1 << 6,
    MOD_SUPER = 1 << 7
} keyboard_modifiers_t;

struct tty_t;

typedef struct {
    void (*putc)(struct tty_t *tty, char c);
    void (*write)(struct tty_t *tty, const char *str);
    void (*read)(struct tty_t *tty, char *buf, size_t len);
    void (*clear)(struct tty_t *tty);
} tty_interface_t;

typedef int    (*tty_history_count_fn)(void *context);
typedef const char *(*tty_history_at_fn)(void *context, int index);

typedef struct tty_t {
    bool echo;
    bool line_ready;

    int input_length;
    int buffer_index;
    int cursor_index;
    char buffer[TTY_BUFFER_SIZE];

    keyboard_modifiers_t modifiers;

    tty_interface_t method;

    console_t *console;

    /* Command history browsing (up/down arrow). */
    void                       *history_context;
    tty_history_count_fn        history_count;
    tty_history_at_fn           history_at;
    int                         history_index;      /* -1 == not browsing; else entry index */
    char                        history_live[TTY_HISTORY_LINE_SIZE]; /* snapshot of the live line */
    bool                        history_has_live;   /* true once a live snapshot exists */
    uint8_t                     history_line_start; /* VGA column where the input line begins */

    /* Interrupt (Ctrl+C) flag. */
    bool interrupt_requested;
} tty_t;

void tty_init(tty_t *tty);

void tty_handle_event(tty_t *tty, const input_event_t *event);
void tty_update_modifiers(tty_t *tty, const input_event_t *event);

void tty_process_events(tty_t *tty);

/* Register shell-provided history callbacks so the tty can recall entries. */
void tty_set_history_callbacks(tty_t *tty, void *context,
                               tty_history_count_fn count_fn, tty_history_at_fn at_fn);

/* Replace the current input line in-place (used when browsing history). */
void tty_replace_line(tty_t *tty, const char *text);

#endif