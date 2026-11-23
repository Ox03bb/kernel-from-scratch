#include "types.h"
#include "vmm.h"

#include "lib/stdio.h"
#include "panic.h"

void vmm_page_fault_handler() {
    uint32_t fault_addr;

    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));
    char *err;

    if (fault_addr == 0x0) {
        err = "Core Dump: NULL pointer dereference";
    } else {
        err = formatter("Page Fault: invalid memory access at virtual address 0x%08x", fault_addr);
    }

    panic(err);
}