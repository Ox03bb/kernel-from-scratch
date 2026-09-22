#ifndef keyboard_H
#define keyboard_H

#include "keyboard_keys.h"
#include "keyboard_layout.h"
#include "ps2.h"
#include "types.h"

typedef enum {
    K_ACK = 0xFA,
    K_RESEND = 0xFE,
    K_ERROR = 0xFC,
    k_BAT = 0xAA, // Basic Assurance Test
} k_response_t;

void init_keyboard(void);

void reset_keyboard(void);
void set_scaning(bool state);
void select_scancode_set(uint8_t set);
void keyboard_set_layout(const keyboard_layout_t *layout);
char keyboard_translate(keycode_t key, const keyboard_state_t *state);

uint8_t keyboard_get_scan_code_set(void);

void keyboard_irq_handler(void);

#endif