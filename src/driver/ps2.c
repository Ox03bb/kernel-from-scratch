#include "ps2.h"
#include "utils.h"



ps2_status_t ps2_status(void){
    return inb(PS2_STATUS_PORT);
}

void ps2_command(uint8_t cmd){
    outb(PS2_COMMAND_PORT, cmd);
}

uint8_t ps2_read(void){
    return inb(PS2_DATA_PORT);
}

