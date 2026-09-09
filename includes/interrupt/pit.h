#ifndef PIT_H
#define PIT_H

#define PIT_CHANNEL0  0x40
#define PIT_COMMAND   0x43
#define PIT_BASE_FREQ 1193182

#include <stdint.h>

void pit_init(uint32_t frequency);
void pit_set_frequency(uint32_t frequency);
uint16_t pit_calculate_divisor(uint32_t frequency);

#endif