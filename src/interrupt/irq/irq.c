#include "irq.h"
#include "vga.h"
#include "vga_lib.h"
#include "pic.h"

void irq_handler(uint32_t vector) {

    if (vector == 0){
        vga_print("Time IRQ \n");
    }else{
        vga_print("IRQ: ");
        vga_print_hex(vector);
        vga_print("\n");
    }
    
    pic_send_eoi((uint8_t)vector);

}

