#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "keyboard.h"
#include "keyboard_keys.h"
#include "ps2.h"
#include "types.h"

#define KEYBOARD_EVENT_QUEUE_SIZE 256

typedef struct {
    key_event_t events[KEYBOARD_EVENT_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
} keyboard_event_queue_t;

extern keyboard_event_queue_t keyboard_queue;

bool keyboard_queue_push(key_event_t event);
bool keyboard_queue_pop(key_event_t *event);

void keyboard_queue_print();

#endif