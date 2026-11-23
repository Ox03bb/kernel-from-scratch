#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define True  1
#define False 0

typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} registers_t;

typedef struct {
    registers_t registers;
    uint32_t vector;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} interrupt_frame_t;

#endif
