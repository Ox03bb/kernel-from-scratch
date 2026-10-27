#include "vmm.h"
#include "mm/utils.h"
#include "types.h"

#include "panic.h"

#include "stdio.h"

#define BOOTSTRAP_MEMORY_START 0x7000
#define BOOTSTRAP_MEMORY_END   0xA000

static int vmm_map_bootstrap_memory(pde_t *directory) {
    for (uintptr_t addr = BOOTSTRAP_MEMORY_START; addr < BOOTSTRAP_MEMORY_END; addr += PAGE_SIZE) {
        if (vmm_map_page(directory, addr, addr, VMM_PRESENT | VMM_WRITABLE) != 0) {
            return -1;
        }
    }

    return 0;
}

void enable_paging(uint32_t page_dir) {
    asm volatile("mov %0, %%cr3\n\t"
                 "mov %%cr0, %%eax\n\t"
                 "or $0x80000000, %%eax\n\t"
                 "mov %%eax, %%cr0\n\t"
                 :
                 : "r"(page_dir)
                 : "eax", "memory");
}

void load_directory(uint32_t page_dir) {
    asm volatile("mov %0, %%cr3\n\t" : : "r"(page_dir) : "eax", "memory");
}

pde_t *vmm_create_directory() {
    uintptr_t phys_addr = pmm_alloc_frame();

    if (phys_addr == 0)
        return NULL;

    pde_t *directory = (pde_t *)phys_addr;

    for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
        directory[i].raw = 0;
    }

    return directory;
}

void vmm_invalidate_page(uint32_t virtual_addr) {
    asm volatile("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

pte_t *vmm_create_page_table() {
    uintptr_t phys_addr = pmm_alloc_frame();

    if (phys_addr == 0)
        return NULL;

    pte_t *table = (pte_t *)phys_addr;

    for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
        table[i].raw = 0;
    }

    return table;
}

int8_t vmm_map_page(pde_t *directory, uint32_t virtual_addr, uint32_t physical_addr,
                    uint32_t flags) {
    if (directory == NULL)
        return -1;

    /* Both addresses must be page-aligned. */
    if ((virtual_addr & (PAGE_SIZE - 1)) != 0 || (physical_addr & (PAGE_SIZE - 1)) != 0)
        return -1;

    vaddr_t va;
    va.raw = virtual_addr;

    pde_t *pde = &directory[va.pd];

    /* Allocate a page table if this PDE is not present. */
    if (!pde->p) {
        pte_t *table = vmm_create_page_table();

        if (table == NULL)
            return -1;

        uint32_t table_phys = (uint32_t)table;

        pde->raw = 0;
        pde->addr = table_phys >> 12;
        pde->rw = 1;
        pde->us = 0;
        pde->ps = 0;
        pde->p = 1;
    }

    /*
     * This direct cast is valid only under the
     * physical-address-access assumption stated above.
     */
    pte_t *table = (pte_t *)(pde->addr << 12);

    pte_t *pte = &table[va.pt];

    pte->raw = 0;
    pte->addr = physical_addr >> 12;
    pte->rw = (flags & 0x2U) != 0;
    pte->us = (flags & 0x4U) != 0;
    pte->p = 1;

    return 0;
}

int8_t vmm_unmap_page(pde_t *directory, uint32_t virtual_addr) {
    if (directory == NULL)
        return -1;

    if ((virtual_addr & (PAGE_SIZE - 1)) != 0)
        return -1;

    vaddr_t va;
    va.raw = virtual_addr;

    pde_t *pde = &directory[va.pd];

    if (!pde->p)
        return -1;

    pte_t *table = (pte_t *)(pde->addr << 12);
    pte_t *pte = &table[va.pt];

    if (!pte->p)
        return -1;

    pte->raw = 0;

    vmm_invalidate_page(virtual_addr);

    return 0;
}

pte_t *vmm_get_pte(pde_t *directory, vaddr_t va) {

    if (directory == NULL)
        return NULL;

    pde_t *pde = &directory[va.pd];

    if (!pde->p) {
        return NULL;
    }

    pte_t *page_table = (pte_t *)(pde->addr << 12);
    pte_t *pte = &page_table[va.pt];

    if (!pte->p)
        return NULL;

    return pte;
}

uintptr_t vmm_get_phys_addr(pde_t *directory, vaddr_t va, bool *err) {
    *err = false;

    pte_t *p = vmm_get_pte(directory, va);
    if (p == NULL) {
        *err = true;
        return 0;
    }

    return ((uintptr_t)p->addr << 12) | va.offset;
}

bool vmm_is_mapped(pde_t *directory, vaddr_t va) { return vmm_get_pte(directory, va) != NULL; }

void vmm_init(void) {
    pde_t *kernel_directory = vmm_create_directory();

    if (kernel_directory == NULL)
        panic("Failed to create kernel page directory");

    if (vmm_map_kernel(kernel_directory) != 0)
        panic("Failed to map kernel");

    if (vmm_map_bootstrap_memory(kernel_directory) != 0)
        panic("Failed to map bootstrap memory");

    if (vmm_map_vga(kernel_directory) != 0)
        panic("Failed to map VGA");

    enable_paging((uint32_t)kernel_directory);
}