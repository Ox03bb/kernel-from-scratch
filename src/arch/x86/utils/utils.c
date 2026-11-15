#include "utils/utils.h"

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



void pushad(void) { __asm__ volatile("pushad"); }
void pushfd(void) { __asm__ volatile("pushfd"); }

void popad(void) { __asm__ volatile("popad"); }
void popfd(void) { __asm__ volatile("popfd"); }

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

typedef enum {
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_ESI,
    REG_EDI,
    REG_EBP,
    REG_ESP,
    REG_EFLAGS
} register_name_t;

uint32_t reg_v(register_name_t reg) {
    uint32_t value;

    switch (reg) {
    case REG_EAX:
        __asm__ volatile("movl %%eax, %0" : "=r"(value));
        break;
    case REG_EBX:
        __asm__ volatile("movl %%ebx, %0" : "=r"(value));
        break;
    case REG_ECX:
        __asm__ volatile("movl %%ecx, %0" : "=r"(value));
        break;
    case REG_EDX:
        __asm__ volatile("movl %%edx, %0" : "=r"(value));
        break;
    case REG_ESI:
        __asm__ volatile("movl %%esi, %0" : "=r"(value));
        break;
    case REG_EDI:
        __asm__ volatile("movl %%edi, %0" : "=r"(value));
        break;
    case REG_EBP:
        __asm__ volatile("movl %%ebp, %0" : "=r"(value));
        break;
    case REG_ESP:
        __asm__ volatile("movl %%esp, %0" : "=r"(value));
        break;
    case REG_EFLAGS:
        value = read_eflags();
        break;
    default:
        value = 0;
        break;
    }

    return value;
}