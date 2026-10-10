#include "pic.h"
#include "utils.h"

void pic_init(void) {
    pic_remap(0x20, 0x28);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t master_mask;
    uint8_t slave_mask;

    // Save current interrupt masks
    master_mask = inb(PIC1_DATA);
    slave_mask = inb(PIC2_DATA);

    // ICW1: begin initialization
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: set vector offsets
    outb(PIC1_DATA, offset1);
    io_wait();

    outb(PIC2_DATA, offset2);
    io_wait();

    // ICW3: tell master about slave on IRQ2
    outb(PIC1_DATA, 0x04);
    io_wait();

    // ICW3: tell slave its cascade identity
    outb(PIC2_DATA, 0x02);
    io_wait();

    // ICW4: 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore interrupt masks
    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

void irq_set_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void pic_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if (IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

static uint16_t __pic_get_irq_reg(int ocw3) {
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register (IRR) */
uint16_t pic_get_irr(void) { return __pic_get_irq_reg(PIC_READ_IRR); }

/* Returns the combined value of the cascaded PICs in-service register (ISR) */
uint16_t pic_get_isr(void) { return __pic_get_irq_reg(PIC_READ_ISR); }