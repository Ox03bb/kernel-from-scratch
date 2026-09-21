#ifndef KEYBOARD_LAYOUT_H
#define KEYBOARD_LAYOUT_H

#include "keyboard_keys.h"
#include "types.h"


#define KEY_COUNT 256

typedef struct {
    char normal;
    char shift;
} keyboard_layout_entry_t;


typedef struct {
    const char *name;

    keyboard_layout_entry_t keys[KEY_COUNT];
} keyboard_layout_t;


extern const keyboard_layout_t keyboard_layout_us;

char keyboard_layout_translate(
    const keyboard_layout_t *layout,
    keycode_t key,
    const keyboard_state_t *state
);


#endif