#ifndef UTILS_H
#define UTILS_H

#include "thread.h"
#include "types.h"

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

void io_wait(void);

void cli(void);
void sti(void);
void hlt(void);

void pushad(void);
void pushfd(void);
void popad(void);
void popfd(void);

registers_t pushad_r(void);
uint32_t pushfd_r(void);

uint32_t read_eflags(void);

typedef enum {
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_ESI,
    REG_EDI,
    REG_EBP,
    REG_ESP,
    REG_EFLAGS,
    REG_EIP,
    REG_CS,
    REG_DS,
    REG_ES,
    REG_FS,
    REG_GS,
    REG_SS,
    REG_CR0,
    REG_CR2,
    REG_CR3,
    REG_CR4
} register_name_t;

uint32_t reg_v(register_name_t reg);

#endif