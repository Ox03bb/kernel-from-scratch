#include "pit.h"
#include "utils.h"
#include <stdint.h>

void pit_init(uint32_t frequency){
    pit_set_frequency(frequency);
}


void pit_set_frequency(uint32_t frequency){
    uint16_t divisor = pit_calculate_divisor(frequency);
    
    div = divisor;
    freq = frequency;

    outb(PIT_COMMAND, 0x34); // ch 0, mode 2, binary

    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));        // Low byte
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF)); // High byte

}


uint16_t pit_calculate_divisor(uint32_t frequency)
{
    if (frequency == 0)
        return 0;

    uint32_t divisor = PIT_BASE_FREQ / frequency;

    if (divisor < 1)
        divisor = 1;

    if (divisor > 65535) // overflow check
        divisor = 65535;

    return (uint16_t)divisor;
}


uint16_t get_divisor(){
    return div;
}

uint32_t get_frequency(){
    return freq;
}