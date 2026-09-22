#ifndef HANDLER_H
#define HANDLER_H

#include "keyboard.h"
#include "keyboard_keys.h"
#include "ps2.h"
#include "stdio.h"
#include "types.h"

extern scancode_state_t scan_state;

void keyboard_irq_handler(void);

void keyboard_process_scancode(uint8_t data);
void keyboard_handle_key(keycode_t key, key_action_t action);
void keyboard_update_state(keycode_t key, key_action_t action);

// queue for keyboard events

#endif