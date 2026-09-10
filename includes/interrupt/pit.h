#ifndef PIT_H
#define PIT_H

#define PIT_CHANNEL0  0x40
#define PIT_CHANNEL1  0x41
#define PIT_CHANNEL2  0x42
#define PIT_COMMAND   0x43
#define PIT_BASE_FREQ 1193182

#include <stdint.h>

/* Channels */
#define PIT_CHANNEL_0 0
#define PIT_CHANNEL_1 1
#define PIT_CHANNEL_2 2

/* Access modes */
#define PIT_ACCESS_LATCH    0x00
#define PIT_ACCESS_LOBYTE   0x10
#define PIT_ACCESS_HIBYTE   0x20
#define PIT_ACCESS_LOHI     0x30

/* Operating modes */
#define PIT_MODE_0 0x00
#define PIT_MODE_1 0x02
#define PIT_MODE_2 0x04
#define PIT_MODE_3 0x06
#define PIT_MODE_4 0x08
#define PIT_MODE_5 0x0A

/* Binary mode */
#define PIT_BINARY 0x00

extern uint16_t div;
extern uint32_t freq;


void pit_init(uint32_t frequency);
void pit_set_frequency(uint32_t frequency);
uint16_t pit_calculate_divisor(uint32_t frequency);
uint16_t get_divisor();
uint32_t get_frequency();

#endif