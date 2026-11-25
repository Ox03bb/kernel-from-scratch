#include "tty.h"
#include "driver/vga.h"
#include "keyboard/event_queue.h"
#include "keyboard_keys.h"

#include "console.h"

void tty_putc(tty_t *tty, char c);
void tty_write(tty_t *tty, const char *str);
void tty_read(tty_t *tty, char *buf, size_t len);
void tty_clear(tty_t *tty);

static void tty_history_previous(tty_t *tty);
static void tty_history_next(tty_t *tty);

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

    tty->history_context = NULL;
    tty->history_count = NULL;
    tty->history_at = NULL;
    tty->history_index = -1;
    tty->history_has_live = false;
    tty->history_live[0] = '\0';
    tty->history_line_start = 0;
    tty->interrupt_requested = false;

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
             * Request an interrupt so a blocking command (e.g. sleep)
             * can abort early.
             */
            tty->interrupt_requested = true;
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

    case KEY_UP:
        tty_history_previous(tty);
        return;

    case KEY_DOWN:
        tty_history_next(tty);
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

/* -------------------------------------------------------------------------- */
/* Command history browsing (up/down arrow)                                   */
/* -------------------------------------------------------------------------- */

static void tty_history_previous(tty_t *tty) {
    int count;

    if (tty == NULL || tty->history_count == NULL || tty->history_at == NULL)
        return;

    count = tty->history_count(tty->history_context);

    /* Snapshot the live line the first time the user starts browsing. */
    if (!tty->history_has_live) {
        size_t i;

        for (i = 0; i < (size_t)tty->buffer_index && i < sizeof(tty->history_live) - 1; i++)
            tty->history_live[i] = tty->buffer[i];
        tty->history_live[i] = '\0';
        tty->history_has_live = true;
        tty->history_index = count;

        /* Remember the screen column where the input line text begins so a
         * recalled line can be redrawn even if the cursor was mid-line. */
        {
            uint8_t cursor_x = vga_get_cursor_x();
            tty->history_line_start = cursor_x >= (uint8_t)tty->cursor_index
                                          ? (uint8_t)(cursor_x - (uint8_t)tty->cursor_index)
                                          : 0;
        }
    }

    /* Move to the previous entry (index 0 is the oldest; stay there). */
    if (tty->history_index > 0) {
        const char *entry;

        tty->history_index--;
        entry = tty->history_at(tty->history_context, tty->history_index);
        if (entry != NULL)
            tty_replace_line(tty, entry);
    }
}

static void tty_history_next(tty_t *tty) {
    int count;

    if (tty == NULL || !tty->history_has_live)
        return;

    count = (tty->history_count != NULL) ? tty->history_count(tty->history_context) : 0;

    if (tty->history_index < count)
        tty->history_index++;

    if (tty->history_index == count) {
        /* Back at the live line: restore the snapshot and stop browsing. */
        tty_replace_line(tty, tty->history_live);
        tty->history_has_live = false;
        tty->history_index = -1;
    } else {
        const char *entry = tty->history_at(tty->history_context, tty->history_index);

        if (entry != NULL)
            tty_replace_line(tty, entry);
    }
}

void tty_set_history_callbacks(tty_t *tty, void *context, tty_history_count_fn count_fn,
                               tty_history_at_fn at_fn) {
    if (tty == NULL)
        return;

    tty->history_context = context;
    tty->history_count = count_fn;
    tty->history_at = at_fn;
    tty->history_index = -1;
    tty->history_has_live = false;
    tty->history_live[0] = '\0';
    tty->history_line_start = 0;
}

void tty_replace_line(tty_t *tty, const char *text) {
    size_t len;
    size_t i;
    uint8_t x;
    uint8_t y;

    if (tty == NULL || text == NULL || tty->console == NULL)
        return;

    len = 0;
    while (text[len] != '\0' && len < TTY_BUFFER_SIZE - 1)
        len++;

    /* Move the VGA cursor to the start of the input line, then echo. */
    {
        uint8_t y = vga_get_cursor_y();
        vga_set_cursor(tty->history_line_start, y);
    }

    /* Clear the remainder of the screen line so stale characters vanish. */
    x = vga_get_cursor_x();
    y = vga_get_cursor_y();
    vga_clear_region(x, y, (uint8_t)(VGA_WIDTH - x), 1, DEFUALT_B | DEFUALT_F);

    /* Echo the new text; vga_print_char advances the VGA cursor for us. */
    if (tty->echo && tty->console->method.write != NULL) {
        for (i = 0; i < len; i++) {
            char ch[2] = {text[i], '\0'};
            tty->console->method.write(tty->console, ch);
        }
    }

    /* Update tty state to match the replaced line; cursor lands at the end. */
    for (i = 0; i < len; i++)
        tty->buffer[i] = text[i];
    tty->buffer[len] = '\0';
    tty->buffer_index = (int)len;
    tty->cursor_index = (int)len;
    tty->input_length = (int)len;
}