#include "utils.h"

void outb(uint16_t port, uint8_t value) {
    // a mean the the value is in the AL register
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));

    return value;
}

void cli(void) { __asm__ volatile("cli"); }

void sti(void) { __asm__ volatile("sti"); }

void hlt(void) { __asm__ volatile("hlt"); }

void io_wait(void) { // delay for I/O operation

    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

uint32_t read_eflags(void) {
    uint32_t flags;
    asm volatile("pushf\n"
                 "pop %0"
                 : "=r"(flags));
    return flags;
}