#include "isr.h"
#include "panic.h"
#include "vga.h"

void isr_handler(uint32_t vector) {
    if (vector <= 31) {
        vga_print("\n");
        exception(EXCEPTIONS[vector]);
    } else {
        panic("Unknown Exception");
    }
}
