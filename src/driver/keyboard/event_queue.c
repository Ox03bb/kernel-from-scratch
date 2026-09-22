#include "event_queue.h"
#include "stdio.h"

// Input event queue

keyboard_event_queue_t keyboard_queue = {0};
input_event_t input_queue[KEYBOARD_EVENT_QUEUE_SIZE];
uint32_t input_queue_head = 0;
uint32_t input_queue_tail = 0;

void keyboard_queue_push(key_event_t event) {
    uint32_t next = (keyboard_queue.head + 1) % KEYBOARD_EVENT_QUEUE_SIZE;

    if (next == keyboard_queue.tail) {
        return;
    }

    keyboard_queue.events[keyboard_queue.head] = event;
    keyboard_queue.head = next;
}

bool keyboard_queue_pop(key_event_t *event) {
    if (keyboard_queue.head == keyboard_queue.tail) {
        return false;
    }

    *event = keyboard_queue.events[keyboard_queue.tail];
    keyboard_queue.tail = (keyboard_queue.tail + 1) % KEYBOARD_EVENT_QUEUE_SIZE;
    return true;
}

void keyboard_queue_clear(void) {
    keyboard_queue.head = 0;
    keyboard_queue.tail = 0;
}

void input_queue_push(input_event_t event) {
    uint32_t next = (input_queue_head + 1) % KEYBOARD_EVENT_QUEUE_SIZE;

    if (next == input_queue_tail) {
        return; /* queue full */
    }

    input_queue[input_queue_head] = event;
    input_queue_head = next;
}

bool input_queue_pop(input_event_t *event) {
    if (input_queue_head == input_queue_tail) {
        if (event != NULL) {
            *event = (input_event_t){.type = (input_type_t)-1, .character = '\0'};
        }
        return false; /* queue empty */
    }

    *event = input_queue[input_queue_tail];
    input_queue_tail = (input_queue_tail + 1) % KEYBOARD_EVENT_QUEUE_SIZE;
    return true;
}

void input_queue_print() {
    printf("\033[32mInput Queue: \033[0m head=%d, tail=%d\n", input_queue_head, input_queue_tail);
    uint32_t count = (input_queue_head - input_queue_tail + KEYBOARD_EVENT_QUEUE_SIZE) % KEYBOARD_EVENT_QUEUE_SIZE;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t index = (input_queue_tail + i) % KEYBOARD_EVENT_QUEUE_SIZE;
        input_event_t event = input_queue[index];
        printf("Event[%d]: type=%d, character=%c\n", i, event.type, event.character);
    }
}

void input_queue_print_last() {
    if (input_queue_head == input_queue_tail) {
        printf("\033[31mInput Queue: \033[0m Empty\n");
        return;
    }

    uint32_t last = (input_queue_head - 1 + KEYBOARD_EVENT_QUEUE_SIZE) % KEYBOARD_EVENT_QUEUE_SIZE;
    input_event_t event = input_queue[last];
    printf("\033[32mInput Queue:\033[0m Last event[%d] : type=%d, character=%c\n", last, event.type,
           event.character);
}

void input_queue_clear() {
    input_queue_head = 0;
    input_queue_tail = 0;
}
