#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init(uint32_t frequency);

void timer_irq_handler(void);

uint32_t timer_get_ticks(void);

uint32_t timer_get_seconds(void);

uint32_t timer_get_frequency(void);

void timer_sleep_m(uint32_t milliseconds);
void timer_sleep_s(uint32_t seconds);
void timer_sleep(float seconds);

#endif