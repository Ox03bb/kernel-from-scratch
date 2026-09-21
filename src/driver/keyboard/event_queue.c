#include "event_queue.h"
#include "stdio.h"

keyboard_event_queue_t keyboard_queue = {0};

bool keyboard_queue_push(key_event_t event) {
    uint32_t next = (keyboard_queue.head + 1) % KEYBOARD_EVENT_QUEUE_SIZE;

    if (next == keyboard_queue.tail) {
        return false; /* queue full */
    }

    keyboard_queue.events[keyboard_queue.head] = event;
    keyboard_queue.head = next;

    return true;
}

bool keyboard_queue_pop(key_event_t *event) {
    if (keyboard_queue.head == keyboard_queue.tail) {
        return false; /* queue empty */
    }

    *event = keyboard_queue.events[keyboard_queue.tail];

    keyboard_queue.tail = (keyboard_queue.tail + 1) % KEYBOARD_EVENT_QUEUE_SIZE;

    return true;
}

void keyboard_queue_print() {

    printf("\033[32mKeyboard Queue: \033[0m head=%d, tail=%d\n", keyboard_queue.head,
           keyboard_queue.tail);
    for (uint32_t i = 0; i < keyboard_queue.head - keyboard_queue.tail; i++) {
        key_event_t event = keyboard_queue.events[i];
        printf("Event[%d]: key=%d, action=%d\n", i, event.key, event.action);
    }
}

void keyboard_queue_print_last() {
    if (keyboard_queue.head == keyboard_queue.tail) {
        printf("\033[31mKeyboard Queue: \033[0m Empty\n");
        return;
    }

    uint32_t last = (keyboard_queue.head - 1 + KEYBOARD_EVENT_QUEUE_SIZE) % KEYBOARD_EVENT_QUEUE_SIZE;
    key_event_t event = keyboard_queue.events[last];
    printf("\033[32mKeyboard Queue:\033[0m Last event[%d] : key=%d, action=%d\n", last, event.key, event.action);
}