#include "handler.h"
#include "keyboard.h"
#include "keyboard/event_queue.h"
#include "ps2.h"
#include "utils.h"

#include "stdio.h"

scancode_state_t scan_state = SCAN_STATE_NORMAL;

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
    keyboard_queue_print();
}