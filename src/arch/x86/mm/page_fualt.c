#include "types.h"
#include "vmm.h"

#include "lib/stdio.h"
#include "panic.h"

void vmm_page_fault_handler() {
    uint32_t fault_addr;

    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    char *err = formatter("page fault in the virtual address: `0x%x`", fault_addr);
    panic(err);
}