#include "timer.h"
#include "pit.h"
#include "vga.h"

static uint32_t ticks = 0;
static uint32_t timer_frequency = 0;

void timer_init(uint32_t frequency) {
    timer_frequency = frequency;
    pit_init(frequency);
}

void timer_irq_handler(void) { ticks++; }

uint32_t timer_get_ticks(void) { return ticks; }

uint32_t timer_get_seconds(void) { return ticks / timer_frequency; }

uint32_t timer_get_frequency(void) { return timer_frequency; }

void timer_sleep_m(uint32_t milliseconds) {
    uint32_t start = timer_get_ticks();
    uint32_t wait_ticks = ((uint32_t)milliseconds * timer_frequency) / 1000;

    while (timer_get_ticks() - start < wait_ticks) {
    }
}

void timer_sleep_s(uint32_t seconds) { timer_sleep_m(seconds * 1000); }

void timer_sleep(float seconds) { timer_sleep_m((uint32_t)(seconds * 1000)); }