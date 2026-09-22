#include "tty.h"
#include "keyboard/event_queue.h"
#include "keyboard_keys.h"
#include "driver/vga.h"

void init(tty_t *tty){
    tty->buffer_index = 0;
}

void putc(tty_t *tty, char c){
    tty->buffer[tty->buffer_index] = c;
    tty->buffer_index++;
}

void read(tty_t *tty, char *buf, size_t len){
    int i = 0;
    while (i < len && tty->buffer[i] != '\0'){
        buf[i] = tty->buffer[i];
        i++;
    }
    buf[i] = '\0';
}

void write(tty_t *tty, const char *buf){
    int i = 0;
    while (buf[i] != '\0'){
        putc(tty, buf[i]);
        i++;
    }
}

void clear(tty_t *tty){
    int i = 0;
    while (i < TTY_BUFFER_SIZE){
        tty->buffer[i] = '\0';
        i++;
    }
    tty->input_length = 0;
}


void tty_update_modifiers(tty_t *tty, const input_event_t *event)
{
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

void tty_handle_event(tty_t *tty, const input_event_t *event)
{
    if (tty == NULL || event == NULL)
        return;

    /*
     * Update Shift/Ctrl/Alt/Caps/Num/Scroll state.
     */
    tty_update_modifiers(tty, event);

    /*
     * Key releases do not produce normal terminal input.
     */
    if (event->action != KEY_PRESSED)
        return;

    /*
     * Modifier and lock keys have already been handled above.
     */
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

    /*
     * Terminal control combinations.
     */
    if (tty->modifiers & MOD_CTRL) {
        switch (event->key) {

        case KEY_C:
            /* Ctrl+C */
            return;

        default:
            break;
        }
    }

    /*
     * Handle special terminal keys.
     */
    switch (event->key) {

    case KEY_BACKSPACE:
        if (tty->buffer_index > 0) {
            tty->buffer_index--;
            tty->input_length--;

            tty->buffer[tty->buffer_index] = '\0';
        }
        return;

    case KEY_ENTER:
        if (tty->buffer_index < TTY_BUFFER_SIZE - 1) {
            tty->buffer[tty->buffer_index++] = '\n';
            tty->buffer[tty->buffer_index] = '\0';
            tty->input_length++;
            tty->line_ready = true;
        }
        return;

    default:
        break;
    }

    /*
     * Convert the key into a character.
     *
     * This should come from your keyboard-layout layer.
     */
    char c = event->character;

    if (c == '\0')
        return;

    /*
     * Prevent buffer overflow.
     */

    if (tty->buffer_index >= TTY_BUFFER_SIZE - 1)
        return;

    tty->buffer[tty->buffer_index++] = c;
    tty->buffer[tty->buffer_index] = '\0';
    tty->input_length++;
}