#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

void io_wait(void);

void cli(void);
void sti(void);
void hlt(void);

uint32_t read_eflags(void);

#endif