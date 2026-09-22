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

void keyboard_queue_push(key_event_t event);
bool keyboard_queue_pop(key_event_t *event);
void keyboard_queue_clear(void);

typedef enum {
    INPUT_CHAR,
    INPUT_ENTER,
    INPUT_BACKSPACE,
    INPUT_TAB,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
} input_type_t;

typedef struct {
    input_type_t type;
    char character;
} input_event_t;

void input_queue_push(input_event_t event);
bool input_queue_pop(input_event_t *event);
void input_queue_print();
void input_queue_print_last();
void input_queue_clear();

extern input_event_t input_queue[KEYBOARD_EVENT_QUEUE_SIZE];

#endif