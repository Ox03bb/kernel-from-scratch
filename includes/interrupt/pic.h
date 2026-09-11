#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */

#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)

#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 1)

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01

#define ICW4_8086 0x01

#define PIC_EOI 0x20

#define PIC_READ_IRR 0x0a /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b /* OCW3 irq service next CMD read */

void pic_init(void);
void pic_remap(uint8_t offset_master, uint8_t offset_slave);

void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);

void pic_send_eoi(uint8_t irq);
void pic_disable(void);

uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif