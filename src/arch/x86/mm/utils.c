#include "panic.h"

#include "stdio.h"

#include "mm/pmm.h"
#include "mm/vmm.h"

extern uintptr_t kernel_start_addr;
extern uintptr_t kernel_end_addr;

int vmm_map_kernel(pde_t *directory) {
    uintptr_t kernel_start = (uintptr_t)&kernel_start_addr;
    uintptr_t kernel_end = (uintptr_t)&kernel_end_addr;

    if (directory == NULL || kernel_start >= kernel_end)
        return -1;

    uintptr_t start = kernel_start & ~(uintptr_t)(PAGE_SIZE - 1);

    uintptr_t end = (kernel_end + PAGE_SIZE - 1) & ~(uintptr_t)(PAGE_SIZE - 1);

    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        if (vmm_map_page(directory, (uint32_t)addr, (uint32_t)addr,
                         VMM_PRESENT | VMM_WRITABLE) != 0) {
            return -2;
        }
    }

    return 0;
}

int vmm_map_vga(pde_t *directory) {
    if (directory == NULL)
        return -1;

    if (vmm_map_page(directory, 0xB8000, 0xB8000, VMM_PRESENT | VMM_WRITABLE) != 0) {
        return -2;
    }

    return 0;
}