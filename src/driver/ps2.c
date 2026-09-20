#include "ps2.h"
#include "utils.h"

// ---------------------------------------------------------------------
ps2_status_t ps2_status(void) {
    ps2_status_t status;
    status.raw = inb(PS2_STATUS_PORT);
    return status;
}

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

void ps2_flush_output(void) {
    while (ps2_output_buffer_full()) {
        (void)ps2_read();
    }
}
// ---------------------------------------------------------------------

void ps2_command(uint8_t cmd) { outb(PS2_COMMAND_PORT, cmd); }

uint8_t ps2_read(void) { return inb(PS2_DATA_PORT); }

void ps2_write(uint8_t data) {
    ps2_wait_input_clear();
    outb(PS2_DATA_PORT, data);
}

uint8_t ps2_read_config(void) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_READ_CONFIG);
    ps2_wait_output_full();
    return ps2_read();
}

ps2_config_t ps2_get_config(void) {
    ps2_config_t config;
    config.raw = ps2_read_config();
    return config;
}

void ps2_write_config(uint8_t config) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_WRITE_CONFIG);
    ps2_wait_input_clear();
    ps2_write(config);
}

void ps2_set_config(ps2_config_t config) { ps2_write_config(config.raw); }

bool ps2_test_controller(void) {
    ps2_flush_output();
    ps2_wait_input_clear();
    ps2_command(PS2_CC_SELF_TEST);
    ps2_wait_output_full();
    return ps2_read() == 0x55;
}

bool ps2_test_port1(void) {
    ps2_flush_output();
    ps2_wait_input_clear();
    ps2_command(PS2_CC_TEST_PORT1);
    ps2_wait_output_full();
    return ps2_read() == 0x00;
}

bool ps2_test_port2(void) {
    ps2_flush_output();
    ps2_wait_input_clear();
    ps2_command(PS2_CC_TEST_PORT2);
    ps2_wait_output_full();
    return ps2_read() == 0x00;
}

void ps2_disable_port1(void) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_DISABLE_PORT1);
}

void ps2_enable_port1(void) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_ENABLE_PORT1);
}

void ps2_disable_port2(void) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_DISABLE_PORT2);
}

void ps2_enable_port2(void) {
    ps2_wait_input_clear();
    ps2_command(PS2_CC_ENABLE_PORT2);
}

void ps2_init(void) {
    ps2_flush_output();
    ps2_disable_port1();
    ps2_disable_port2();

    if (!ps2_test_controller()) {
        return;
    }

    uint8_t config = ps2_read_config();
    config &= ~(1u << 0);
    config &= ~(1u << 1);
    config &= ~(1u << 6);
    ps2_write_config(config);

    if (ps2_test_port1()) {
        ps2_enable_port1();
    }

    if (ps2_test_port2()) {
        ps2_enable_port2();
    }
}

uint8_t keyboard_get_scan_code_set(void) {
    uint8_t response;

    ps2_write(0xF0); // Command to get the current scan-code set.

    response = ps2_read();

    if (response != 0xFA) {
        return 0;
    }

    ps2_write(0x00); // Request to get the current scan-code set.

    response = ps2_read();

    if (response != 0xFA) {
        return 0;
    }

    return ps2_read();
}