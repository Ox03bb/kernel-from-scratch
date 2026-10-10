#include "tty.h"
#include "driver/vga.h"
#include "keyboard/event_queue.h"
#include "keyboard_keys.h"

#include "console.h"

void tty_putc(tty_t *tty, char c);
void tty_write(tty_t *tty, const char *str);
void tty_read(tty_t *tty, char *buf, size_t len);
void tty_clear(tty_t *tty);

static const tty_interface_t tty_default_interface = {
    .putc = tty_putc,
    .write = tty_write,
    .read = tty_read,
    .clear = tty_clear,
};

console_t vga_console;

void tty_init(tty_t *tty) {

    if (tty == NULL)
        return;

    tty->echo = true;
    tty->line_ready = false;

    tty->input_length = 0;
    tty->buffer_index = 0;
    tty->cursor_index = 0;

    tty->modifiers = MOD_NONE;

    for (size_t i = 0; i < TTY_BUFFER_SIZE; i++)
        tty->buffer[i] = '\0';

    tty->method = tty_default_interface;

    tty->console = &vga_console;

    console_init(tty->console);
}

void tty_putc(tty_t *tty, char c) {
    if (tty == NULL)
        return;

    if (tty->buffer_index >= TTY_BUFFER_SIZE - 1)
        return;

    tty->buffer[tty->buffer_index++] = c;
    tty->buffer[tty->buffer_index] = '\0';

    tty->input_length++;
}

void tty_write(tty_t *tty, const char *str) {
    if (tty == NULL || str == NULL)
        return;

    if (tty->console == NULL || tty->console->method.write == NULL)
        return;

    for (size_t i = 0; str[i] != '\0'; i++) {
        char ch[2] = {str[i], '\0'};
        tty->console->method.write(tty->console, ch);
    }
}

void tty_read(tty_t *tty, char *buf, size_t len) {
    if (tty == NULL || buf == NULL || len == 0)
        return;

    size_t i = 0;

    /*
     * Reserve one byte for '\0'.
     */
    while (i < len - 1 && tty->buffer[i] != '\0') {
        buf[i] = tty->buffer[i];
        i++;
    }

    buf[i] = '\0';
}

void tty_clear(tty_t *tty) {
    if (tty == NULL)
        return;

    for (size_t i = 0; i < TTY_BUFFER_SIZE; i++)
        tty->buffer[i] = '\0';

    tty->buffer_index = 0;
    tty->cursor_index = 0;
    tty->input_length = 0;
    tty->line_ready = false;

    if (tty->console != NULL && tty->console->method.clear != NULL)
        tty->console->method.clear(tty->console);
}

void tty_update_modifiers(tty_t *tty, const input_event_t *event) {
    if (tty == NULL || event == NULL) {
        return;
    }

    bool pressed = event->action == KEY_PRESSED;

    switch (event->key) {

    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:
        if (pressed)
            tty->modifiers |= MOD_SHIFT;
        else
            tty->modifiers &= ~MOD_SHIFT;
        break;

    case KEY_LEFT_CTRL:
    case KEY_RIGHT_CTRL:
        if (pressed)
            tty->modifiers |= MOD_CTRL;
        else
            tty->modifiers &= ~MOD_CTRL;
        break;

    case KEY_LEFT_ALT:
        if (pressed)
            tty->modifiers |= MOD_ALT;
        else
            tty->modifiers &= ~MOD_ALT;
        break;

    case KEY_RIGHT_ALT:
        if (pressed)
            tty->modifiers |= MOD_ALTGR;
        else
            tty->modifiers &= ~MOD_ALTGR;
        break;

    case KEY_CAPS_LOCK:
        if (pressed)
            tty->modifiers ^= MOD_CAPS;
        break;

    case KEY_NUM_LOCK:
        if (pressed)
            tty->modifiers ^= MOD_NUM;
        break;

    case KEY_SCROLL_LOCK:
        if (pressed)
            tty->modifiers ^= MOD_SCROLL;
        break;

    default:
        break;
    }
}

void tty_handle_event(tty_t *tty, const input_event_t *event) {
    if (tty == NULL || event == NULL)
        return;

    tty_update_modifiers(tty, event);

    if (event->action != KEY_PRESSED)
        return;

    switch (event->key) {

    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:

    case KEY_LEFT_CTRL:
    case KEY_RIGHT_CTRL:

    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:

    case KEY_CAPS_LOCK:
    case KEY_NUM_LOCK:
    case KEY_SCROLL_LOCK:

        return;

    default:
        break;
    }

    if (tty->modifiers & MOD_CTRL) {

        switch (event->key) {

        case KEY_C:
            /*
             * Ctrl+C
             *
             * Process/signal handling can be added later.
             */
            return;

        default:
            break;
        }
    }

    switch (event->key) {

    case KEY_LEFT:
        if (tty->cursor_index > 0) {
            tty->cursor_index--;
            vga_cursor_left();
        }
        return;

    case KEY_RIGHT:
        if (tty->cursor_index < tty->buffer_index) {
            tty->cursor_index++;
            vga_cursor_right();
        }
        return;

    case KEY_BACKSPACE:

        if (tty->cursor_index > 0) {
            uint16_t delete_position = vga_get_cursor_position();

            for (int i = tty->cursor_index - 1; i < tty->buffer_index; i++)
                tty->buffer[i] = tty->buffer[i + 1];

            tty->cursor_index--;
            tty->buffer_index--;

            if (tty->input_length > 0)
                tty->input_length--;

            if (tty->echo) {
                vga_cursor_left();
                delete_position = vga_get_cursor_position();

                for (int i = tty->cursor_index; i < tty->buffer_index; i++) {
                    char output[2] = {tty->buffer[i], '\0'};
                    tty->console->method.write(tty->console, output);
                }

                vga_print_char(' ');
                vga_set_cursor_position(delete_position);
            }

            tty->buffer[tty->buffer_index] = '\0';
        }

        return;

    case KEY_ENTER:

        if (tty->buffer_index < TTY_BUFFER_SIZE - 1) {

            if (tty->echo) {
                vga_set_cursor_position(vga_get_cursor_position() +
                                        (tty->buffer_index - tty->cursor_index));
            }

            tty->buffer[tty->buffer_index++] = '\n';
            tty->buffer[tty->buffer_index] = '\0';

            tty->input_length++;
            tty->cursor_index = tty->buffer_index;
            tty->line_ready = true;

            if (tty->echo)
                tty->console->method.write(tty->console, "\n");
        }

        return;

    default:
        break;
    }

    char c = event->character;

    if (c == '\0')
        return;

    if (tty->buffer_index < TTY_BUFFER_SIZE - 1) {
        uint16_t cursor_position = vga_get_cursor_position();

        for (int i = tty->buffer_index; i > tty->cursor_index; i--)
            tty->buffer[i] = tty->buffer[i - 1];

        tty->buffer[tty->cursor_index++] = c;
        tty->buffer_index++;
        tty->buffer[tty->buffer_index] = '\0';

        tty->input_length++;

        if (tty->echo) {
            char output[2] = {c, '\0'};
            tty->console->method.write(tty->console, output);

            for (int i = tty->cursor_index; i < tty->buffer_index; i++) {
                output[0] = tty->buffer[i];
                tty->console->method.write(tty->console, output);
            }

            vga_set_cursor_position(cursor_position + 1);
        }
    }
}

// entry point

void tty_process_events(tty_t *tty) {
    input_event_t event;

    while (input_queue_pop(&event)) {
        tty_handle_event(tty, &event);
    }
}