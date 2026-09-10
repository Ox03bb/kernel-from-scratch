#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

#define IDT_INTERRUPT_GATE   0x8E
#define KERNEL_CODE_SELECTOR 0x08

extern uint32_t isr_table[256];
extern uint32_t irq_table[16];

struct idt_entry {
    uint16_t offset_low;  // handler address bits 0..15
    uint16_t selector;    // code segment selector
    uint8_t zero;         // must be 0
    uint8_t type_attr;    // P, DPL, gate type
    uint16_t offset_high; // handler address bits 16..31
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// __attribute__((packed)) for eliminate padding bytes

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr);

void idt_init_descriptor(void);
void idt_init(void);

void idt_clear(void);

#endif