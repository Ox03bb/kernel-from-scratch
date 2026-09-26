#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "keyboard.h"
#include "keyboard_keys.h"
#include "ps2.h"
#include "types.h"

#define INPUT_EVENT_QUEUE_SIZE 256

typedef enum {
    INPUT_EVENT_KEY,
    INPUT_EVENT_CHAR,
} input_event_type_t;

typedef struct {
    input_event_type_t type;
    keycode_t key;
    key_action_t action;
    bool shift;
    bool ctrl;
    bool alt;
    char character;
} input_event_t;

typedef struct {
    input_event_t events[INPUT_EVENT_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
} input_event_queue_t;

extern input_event_queue_t input_event_queue;

void input_queue_init(void);
void input_queue_push(input_event_t event);
bool input_queue_pop(input_event_t *event);
bool input_queue_is_empty(void);
bool input_queue_is_full(void);
void input_queue_print();
void input_queue_print_last();
void input_queue_clear(void);

#endif