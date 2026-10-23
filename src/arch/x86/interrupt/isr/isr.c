#include "isr.h"
#include "mm/vmm.h"
#include "panic.h"
#include "vga.h"

void isr_handler(uint32_t vector) {
    if (vector <= 31) {
        vga_print("\n");
        if (vector == 14) {
            vmm_page_fault_handler();
        }

        exception(EXCEPTIONS[vector]);
    } else {
        panic("Unknown Exception");
    }
}
