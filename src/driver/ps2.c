#include "ps2.h"
#include "utils.h"

// ---------------------------------------------------------------------
ps2_status_t ps2_status(void) { return inb(PS2_STATUS_PORT); }

bool ps2_output_buffer_full(void) { return ps2_status().output_buffer_full; }

bool ps2_input_buffer_full(void) { return ps2_status().input_buffer_full; }

bool ps2_system_flag(void) { return ps2_status().system_flag; }

bool ps2_command_data(void) { return ps2_status().command_data; }

bool ps2_timeout_error(void) { return ps2_status().timeout_error; }

bool ps2_parity_error(void) { return ps2_status().parity_error; }

bool ps2_zero(void) { return ps2_status().zero; }

bool ps2_wait_input_clear(void) {
    while (ps2_input_buffer_full())
        ;
    return true;
}

bool ps2_wait_output_full(void) {
    while (!ps2_output_buffer_full())
        ;
    return true;
}
// ---------------------------------------------------------------------

void ps2_command(uint8_t cmd) { outb(PS2_COMMAND_PORT, cmd); }

uint8_t ps2_read(void) { return inb(PS2_DATA_PORT); }
