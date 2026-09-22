#include "event_queue.h"
#include "stdio.h"

// Input event queue

input_event_t input_queue[KEYBOARD_EVENT_QUEUE_SIZE];
uint32_t input_queue_head = 0;
uint32_t input_queue_tail = 0;

void input_queue_push(input_event_t event) {
    uint32_t next = (input_queue_head + 1) % KEYBOARD_EVENT_QUEUE_SIZE;

    if (next == input_queue_tail) {
        return; /* queue full */
    }

    input_queue[input_queue_head] = event;
    input_queue_head = next;
}

void input_queue_pop(input_event_t *event) {
    if (input_queue_head == input_queue_tail) {
        return; /* queue empty */
    }

    *event = input_queue[input_queue_tail];

    input_queue_tail = (input_queue_tail + 1) % KEYBOARD_EVENT_QUEUE_SIZE;
}

void input_queue_print() {
    printf("\033[32mInput Queue: \033[0m head=%d, tail=%d\n", input_queue_head, input_queue_tail);
    for (uint32_t i = 0; i < input_queue_head - input_queue_tail; i++) {
        input_event_t event = input_queue[i];
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
