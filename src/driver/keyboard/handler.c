#include "handler.h"
#include "keyboard.h"
#include "keyboard/event_queue.h"
#include "ps2.h"
#include "utils.h"

scancode_state_t scan_state = SCAN_STATE_NORMAL;

keyboard_state_t keyboard_state = {0};

/* -------------------------------------------------------------------------- */
/* IRQ handler                                                                */
/* -------------------------------------------------------------------------- */

void keyboard_irq_handler(void) {
    if (!ps2_wait_output_full()) {
        return;
    }

    uint8_t data = ps2_read();
    keyboard_process_scancode(data);
}

/* -------------------------------------------------------------------------- */
/* Scan-code processing                                                       */
/* -------------------------------------------------------------------------- */

void keyboard_process_scancode(uint8_t data) {
    keycode_t key;

    switch (scan_state) {

    case SCAN_STATE_NORMAL:

        if (data == 0xF0) {
            scan_state = SCAN_STATE_BREAK;
            return;
        }

        if (data == 0xE0) {
            scan_state = SCAN_STATE_EXTENDED;
            return;
        }

        key = scancode_set2.normal[data];

        if (key == KEY_NONE) {
            return;
        }

        keyboard_handle_key(key, KEY_PRESSED);
        return;

    case SCAN_STATE_BREAK:

        key = scancode_set2.normal[data];

        scan_state = SCAN_STATE_NORMAL;

        if (key == KEY_NONE) {
            return;
        }

        keyboard_handle_key(key, KEY_RELEASED);
        return;

    case SCAN_STATE_EXTENDED:

        if (data == 0xF0) {
            scan_state = SCAN_STATE_EXTENDED_BREAK;
            return;
        }

        key = scancode_set2.extended[data];

        scan_state = SCAN_STATE_NORMAL;

        if (key == KEY_NONE) {
            return;
        }

        keyboard_handle_key(key, KEY_PRESSED);
        return;

    case SCAN_STATE_EXTENDED_BREAK:

        key = scancode_set2.extended[data];

        scan_state = SCAN_STATE_NORMAL;

        if (key == KEY_NONE) {
            return;
        }

        keyboard_handle_key(key, KEY_RELEASED);
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* Key processing                                                             */
/* -------------------------------------------------------------------------- */

void keyboard_handle_key(keycode_t key, key_action_t action) {
    keyboard_update_state(key, action);

    input_event_t input = {0};
    input.type = INPUT_EVENT_KEY;
    input.key = key;
    input.action = action;
    input.shift = keyboard_state.left_shift || keyboard_state.right_shift;
    input.ctrl = keyboard_state.left_ctrl || keyboard_state.right_ctrl;
    input.alt = keyboard_state.left_alt || keyboard_state.right_alt;

    char character = keyboard_translate(key, &keyboard_state);
    if (character != '\0') {
        input.type = INPUT_EVENT_CHAR;
        input.character = character;
        input_queue_push(input);
        return;
    }

    switch (key) {
    case KEY_ENTER:
    case KEY_BACKSPACE:
    case KEY_TAB:
    case KEY_UP:
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_HOME:
    case KEY_END:
    case KEY_DELETE:
    case KEY_ESC:
    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:
    case KEY_LEFT_CTRL:
    case KEY_RIGHT_CTRL:
    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
    case KEY_CAPS_LOCK:
    case KEY_NUM_LOCK:
    case KEY_SCROLL_LOCK:
        input.character = '\0';
        input_queue_push(input);
        return;

    default:
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* Keyboard state                                                             */
/* -------------------------------------------------------------------------- */

void keyboard_update_state(keycode_t key, key_action_t action) {
    bool pressed = action == KEY_PRESSED;

    switch (key) {

    case KEY_LEFT_SHIFT:
        keyboard_state.left_shift = pressed;
        break;

    case KEY_RIGHT_SHIFT:
        keyboard_state.right_shift = pressed;
        break;

    case KEY_LEFT_CTRL:
        keyboard_state.left_ctrl = pressed;
        break;

    case KEY_RIGHT_CTRL:
        keyboard_state.right_ctrl = pressed;
        break;

    case KEY_LEFT_ALT:
        keyboard_state.left_alt = pressed;
        break;

    case KEY_RIGHT_ALT:
        keyboard_state.right_alt = pressed;
        break;

    case KEY_CAPS_LOCK:

        if (pressed) {
            keyboard_state.caps_lock = !keyboard_state.caps_lock;
        }

        break;

    case KEY_NUM_LOCK:

        if (pressed) {
            keyboard_state.num_lock = !keyboard_state.num_lock;
        }

        break;

    case KEY_SCROLL_LOCK:

        if (pressed) {
            keyboard_state.scroll_lock = !keyboard_state.scroll_lock;
        }

        break;

    default:
        break;
    }
}