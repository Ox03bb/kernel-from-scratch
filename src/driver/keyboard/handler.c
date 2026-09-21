#include "handler.h"
#include "keyboard.h"
#include "keyboard/event_queue.h"
#include "ps2.h"
#include "utils.h"
#include "vga.h"

#include "stdio.h"

scancode_state_t scan_state = SCAN_STATE_NORMAL;

keyboard_state_t keyboard_state = {0};

void keyboard_irq_handler(void) {
    if (!ps2_wait_output_full()) {
        return;
    }

    uint8_t data = ps2_read();

    keyboard_process_scancode(data);

    // keycode_t keycode = scancode_set2.normal[data];

    // printf("\n SCAN: %h -> %h", data, keycode);

    // if (data == 0xf0) {
    //     data = ps2_read();
    //     printf("%h\n", data);
    // }
}

void keyboard_process_scancode(uint8_t data) {
    keycode_t key;
    key_event_t event;

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

        event.key = key;
        event.action = KEY_PRESSED;

        keyboard_handle_event(event);

        return;

    case SCAN_STATE_BREAK:

        key = scancode_set2.normal[data];

        scan_state = SCAN_STATE_NORMAL;

        if (key == KEY_NONE) {
            return;
        }

        event.key = key;
        event.action = KEY_RELEASED;

        keyboard_handle_event(event);

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

        event.key = key;
        event.action = KEY_PRESSED;

        keyboard_handle_event(event);

        return;

    case SCAN_STATE_EXTENDED_BREAK:

        key = scancode_set2.extended[data];

        scan_state = SCAN_STATE_NORMAL;

        if (key == KEY_NONE) {
            return;
        }

        event.key = key;
        event.action = KEY_RELEASED;

        keyboard_handle_event(event);

        return;
    }
}


void keyboard_handle_event(key_event_t event) {
    keyboard_queue_push(event);
    printf("\n");
    keyboard_queue_print_last();
}

void keyboard_update_state(const key_event_t *event)
{
    switch (event->key) {

    case KEY_LEFT_SHIFT:
        keyboard_state.left_shift =
            event->action == KEY_PRESSED;
        break;

    case KEY_RIGHT_SHIFT:
        keyboard_state.right_shift =
            event->action == KEY_PRESSED;
        break;

    case KEY_LEFT_CTRL:
        keyboard_state.left_ctrl =
            event->action == KEY_PRESSED;
        break;

    case KEY_RIGHT_CTRL:
        keyboard_state.right_ctrl =
            event->action == KEY_PRESSED;
        break;

    case KEY_LEFT_ALT:
        keyboard_state.left_alt =
            event->action == KEY_PRESSED;
        break;

    case KEY_RIGHT_ALT:
        keyboard_state.right_alt =
            event->action == KEY_PRESSED;
        break;

    default:
        break;
    }
}

// void keyboard_process(void)
// {
//     key_event_t event;

//     while (keyboard_queue_pop(&event)) {
//         keyboard_process_event(&event);
//     }
// }

void keyboard_process_event(const key_event_t *event)
{
    keyboard_update_state(event);

    if (event->action != KEY_PRESSED) {
        return;
    }

    char character = keyboard_translate(event->key, &keyboard_state);

    if (character != '\0') {
        vga_print_char(character);
    }
}