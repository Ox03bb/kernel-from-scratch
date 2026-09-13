#ifndef ISR_H
#define ISR_H

#include "types.h"

char *EXCEPTIONS[] = {
    "Division Error",                 // 00
    "Debug",                          // 01
    "Non-maskable Interrupt",         // 02
    "Breakpoint",                     // 03
    "Overflow",                       // 04
    "Bound Range Exceeded",           // 05
    "Invalid Opcode",                 // 06
    "Device Not Available",           // 07
    "Double Fault",                   // 08
    "Coprocessor Segment Overrun",    // 09
    "Invalid TSS",                    // 10
    "Segment Not Present",            // 11
    "Stack-Segment Fault",            // 12
    "General Protection Fault",       // 13
    "Page Fault",                     // 14
    "-15",                            // 15
    "x87 Floating-Point Exception",   // 16
    "Alignment Check",                // 17
    "Machine Check",                  // 18
    "SIMD Floating-Point Exception",  // 19
    "Virtualization Exception",       // 20
    "Control Protection Exception",   // 21
    "-22",                            // 22
    "-23",                            // 23
    "-24",                            // 24
    "-25",                            // 25
    "-26",                            // 26
    "-27",                            // 27
    "Hypervisor Injection Exception", // 28
    "VMM Communication Exception",    // 29
    "Security Exception",             // 30
    "-31"                             // 31
};

void isr_handler(uint32_t vector);

#endif