#ifndef HANDLER_H
#define HANDLER_H

#include "keyboard_keys.h"
#include "keyboard.h"
#include "ps2.h"
#include "types.h"
#include "stdio.h"

extern scancode_state_t scan_state;


void keyboard_irq_handler(void);

void keyboard_process_scancode(uint8_t data);
void keyboard_handle_event(key_event_t event);


// queue for keyboard events

#endif