#ifndef PS2_H
#define PS2_H

#include "types.h"

#define PS2_DATA_PORT    0x60
#define PS2_STATUS_PORT  0x64
#define PS2_COMMAND_PORT PS2_STATUS_PORT

// control commands
#define PS2_CC_READ_CONFIG       0x20 // read configuration byte
#define PS2_CC_WRITE_CONFIG      0x60 // write configuration byte
#define PS2_CC_SELF_TEST         0xAA // controller self-test
#define PS2_CC_TEST_PORT1        0xAB // test port 1
#define PS2_CC_TEST_PORT2        0xA9 // test port 2
#define PS2_CC_DISABLE_PORT1     0xAD // disable port 1
#define PS2_CC_ENABLE_PORT1      0xAE // enable port 1
#define PS2_CC_DISABLE_PORT2     0xA7 // disable port 2
#define PS2_CC_ENABLE_PORT2      0xA8 // enable port 2
#define PS2_CC_READ_OUTPUT_PORT  0xD0 // read output port
#define PS2_CC_WRITE_OUTPUT_PORT 0xD1 // write output port
#define PS2_CC_WRITE_TO_PORT2    0xD4 // next byte goes to port 2

// device commands
#define PS2_DC_RESET            0xFF // reset device
#define PS2_DC_DISABLE_SCANNING 0xF5 // disable scanning
#define PS2_DC_IDENTIFY_DEVICE  0xF2 // identify device
#define PS2_DC_ENABLE_SCANNING  0xF4 // enable scanning

typedef struct ps2_config_t {
    uint8_t port1_interrupt : 1;
    uint8_t port2_interrupt : 1;
    uint8_t system_flag : 1;
    uint8_t zero1 : 1;
    uint8_t port1_clock : 1;
    uint8_t port2_clock : 1;
    uint8_t translation : 1;
    uint8_t zero2 : 1;
} ps2_config_t;

typedef union {
    uint8_t raw;

    struct {
        uint8_t output_buffer_full : 1; // Bit 0
        uint8_t input_buffer_full : 1;  // Bit 1
        uint8_t system_flag : 1;        // Bit 2
        uint8_t command_data : 1;       // Bit 3
        uint8_t unknown : 1;            // Bit 4
        uint8_t timeout_error : 1;      // Bit 5
        uint8_t parity_error : 1;       // Bit 6
        uint8_t zero : 1;               // Bit 7
    };
} ps2_status_t;

ps2_status_t ps2_status(void);
bool ps2_output_buffer_full(void);
bool ps2_input_buffer_full(void);
bool ps2_system_flag(void);
bool ps2_command_data(void);
bool ps2_timeout_error(void);
bool ps2_parity_error(void);
bool ps2_wait_input_clear(void);
bool ps2_wait_output_full(void);

uint8_t ps2_read(void);
void ps2_command(uint8_t cmd);

// ps2_read();
// ps2_write();
// ps2_wait_input();
// ps2_wait_output();
// ps2_send_device_command();
// ps2_read_config();
// ps2_write_config();
// ps2_status();

#endif