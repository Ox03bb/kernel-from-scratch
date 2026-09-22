#include "event_queue.h"
#include "stdio.h"

input_event_queue_t input_event_queue = {0};

void input_queue_init(void) {
    input_event_queue.head = 0;
    input_event_queue.tail = 0;
}

void input_queue_push(input_event_t event) {
    uint32_t next = (input_event_queue.head + 1) % INPUT_EVENT_QUEUE_SIZE;

    if (next == input_event_queue.tail) {
        return; /* queue full */
    }

    input_event_queue.events[input_event_queue.head] = event;
    input_event_queue.head = next;
}

bool input_queue_pop(input_event_t *event) {
    if (input_event_queue.head == input_event_queue.tail) {
        if (event != NULL) {
            *event = (input_event_t){0};
        }
        return false; /* queue empty */
    }

    if (event != NULL) {
        *event = input_event_queue.events[input_event_queue.tail];
    }

    input_event_queue.tail = (input_event_queue.tail + 1) % INPUT_EVENT_QUEUE_SIZE;
    return true;
}

bool input_queue_is_empty(void) {
    return input_event_queue.head == input_event_queue.tail;
}

bool input_queue_is_full(void) {
    return ((input_event_queue.head + 1) % INPUT_EVENT_QUEUE_SIZE) == input_event_queue.tail;
}

void input_queue_print() {
    printf("\033[32mInput Queue: \033[0m head=%d, tail=%d\n", input_event_queue.head, input_event_queue.tail);
    uint32_t count = (input_event_queue.head - input_event_queue.tail + INPUT_EVENT_QUEUE_SIZE) % INPUT_EVENT_QUEUE_SIZE;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t index = (input_event_queue.tail + i) % INPUT_EVENT_QUEUE_SIZE;
        input_event_t event = input_event_queue.events[index];
        printf("Event[%d]: key=%d action=%d char=%c\n", i, event.key, event.action, event.character);
    }
}

void input_queue_print_last() {
    if (input_queue_is_empty()) {
        printf("\033[31mInput Queue: \033[0m Empty\n");
        return;
    }

    uint32_t last = (input_event_queue.head - 1 + INPUT_EVENT_QUEUE_SIZE) % INPUT_EVENT_QUEUE_SIZE;
    input_event_t event = input_event_queue.events[last];
    printf("\033[32mInput Queue:\033[0m Last event key=%d action=%d char=%c\n", event.key, event.action,
           event.character);
}

void input_queue_clear(void) {
    input_queue_init();
}
