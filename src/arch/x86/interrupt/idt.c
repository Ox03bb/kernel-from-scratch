#include "idt.h"

static struct idt_entry idt[IDT_ENTRIES];
static struct idtr idtr;

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr) {
    idt[vector].offset_low = handler & 0xFFFF;
    idt[vector].selector = selector;
    idt[vector].zero = 0;
    idt[vector].type_attr = type_attr;
    idt[vector].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init_descriptor(void) {
    idtr.base = (uint32_t)&idt;
    idtr.limit = sizeof(idt) - 1;
}

void idt_clear(void) {
    for (uint16_t i = 0; i < IDT_ENTRIES; i++) {
        idt[i] = (struct idt_entry){0};
    }
}

static void idt_load(void) { asm volatile("lidt %0" : : "m"(idtr)); }

void idt_init(void) {
    idt_clear();

    idt_init_descriptor();

    // isr
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, isr_table[i], KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    }

    // irq
    for (uint8_t i = 32; i < 48; i++) {
        idt_set_gate(i, irq_table[i - 32], KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    }

    idt_load();
}
