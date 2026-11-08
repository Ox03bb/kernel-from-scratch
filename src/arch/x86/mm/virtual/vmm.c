#include "vmm.h"
#include "mm/utils.h"
#include "types.h"

#include "panic.h"

#include "stdio.h"

#define BOOTSTRAP_MEMORY_START 0x7000
#define BOOTSTRAP_MEMORY_END   0xA000

pde_t *current_directory = NULL;
static bool recursive_mapping_ready = false;

static pde_t *vmm_get_directory_entry(pde_t *directory, uint32_t index) {
    if (directory == current_directory && recursive_mapping_ready) {
        return &((pde_t *)RECURSIVE_DIRECTORY)[index];
    }

    return &directory[index];
}

static pte_t *vmm_get_page_table(pde_t *directory, uint32_t pd, pde_t *pde) {
    if (directory == current_directory && recursive_mapping_ready) {
        return (pte_t *)(RECURSIVE_TABLES + (pd * PAGE_SIZE));
    }

    return (pte_t *)(pde->addr << 12);
}

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

    if (!recursive_mapping_ready) {
        pte_t *table = (pte_t *)phys_addr;

        for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
            table[i].raw = 0;
        }
    }

    return (pte_t *)phys_addr;
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

    pde_t *pde = vmm_get_directory_entry(directory, va.pd);

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

        pte_t *new_table = vmm_get_page_table(directory, va.pd, pde);
        for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
            new_table[i].raw = 0;
        }
    }

    /*
     * This direct cast is valid only under the
     * physical-address-access assumption stated above.
     */
    pte_t *table = vmm_get_page_table(directory, va.pd, pde);

    pte_t *pte = &table[va.pt];

    pte->raw = 0;
    pte->addr = physical_addr >> 12;
    pte->rw = (flags & 0x2U) != 0;
    pte->us = (flags & 0x4U) != 0;
    pte->p = 1;

    return 0;
}

int8_t vmm_allocate(uint32_t virtual_addr) {
    if (current_directory == NULL || (virtual_addr & (PAGE_SIZE - 1)) != 0)
        return -1;

    vaddr_t va;
    va.raw = virtual_addr;

    if (vmm_is_mapped(current_directory, va))
        return -1;

    uintptr_t physical_addr = pmm_alloc_frame();

    if (physical_addr == 0)
        return -1;

    if (vmm_map_page(current_directory, virtual_addr, (uint32_t)physical_addr,
                     VMM_PRESENT | VMM_WRITABLE) != 0) {
        pmm_free_frame(physical_addr);
        return -1;
    }

    return 0;
}

int8_t vmm_allocate_range(uint32_t start, uint32_t end) {
    if (current_directory == NULL || start >= end || (start & (PAGE_SIZE - 1)) != 0 ||
        (end & (PAGE_SIZE - 1)) != 0)
        return -1;

    uint32_t page_count = (end - start) / PAGE_SIZE;

    /*
     * Check the complete range before allocating frames so a failed request
     * cannot leave a partially mapped virtual range.
     */
    for (uint32_t i = 0; i < page_count; i++) {
        vaddr_t va;
        va.raw = start + (i * PAGE_SIZE);

        if (vmm_is_mapped(current_directory, va))
            return -1;
    }

    uintptr_t physical_start = pmm_alloc_n_frame((int)page_count);

    if (physical_start == 0)
        return -1;

    for (uint32_t i = 0; i < page_count; i++) {
        uint32_t virtual_addr = start + (i * PAGE_SIZE);
        uint32_t physical_addr = (uint32_t)(physical_start + (i * PAGE_SIZE));

        if (vmm_map_page(current_directory, virtual_addr, physical_addr,
                         VMM_PRESENT | VMM_WRITABLE) != 0) {
            for (uint32_t rollback = 0; rollback < i; rollback++)
                vmm_unmap_page(current_directory, start + (rollback * PAGE_SIZE));

            pmm_free_n_frame(physical_start, (int)page_count);
            return -1;
        }
    }

    return 0;
}

int8_t vmm_unmap_page(pde_t *directory, uint32_t virtual_addr) {
    if (directory == NULL)
        return -1;

    if ((virtual_addr & (PAGE_SIZE - 1)) != 0)
        return -1;

    vaddr_t va;
    va.raw = virtual_addr;

    pde_t *pde = vmm_get_directory_entry(directory, va.pd);

    if (!pde->p)
        return -1;

    pte_t *table = vmm_get_page_table(directory, va.pd, pde);
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

    pde_t *pde = vmm_get_directory_entry(directory, va.pd);

    if (!pde->p) {
        return NULL;
    }

    pte_t *page_table = vmm_get_page_table(directory, va.pd, pde);
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
    current_directory = vmm_create_directory();

    if (current_directory == NULL)
        panic("Failed to create kernel page directory");

    /*
     * The last directory entry points back to the directory itself. This
     * exposes the directory at 0xFFFFF000 and every page table at
     * 0xFFC00000 + (directory index * PAGE_SIZE).
     */
    current_directory[PAGE_ENTRIES - 1].raw = 0;
    current_directory[PAGE_ENTRIES - 1].addr = (uint32_t)current_directory >> 12;
    current_directory[PAGE_ENTRIES - 1].rw = 1;
    current_directory[PAGE_ENTRIES - 1].p = 1;

    if (vmm_map_kernel(current_directory) != 0)
        panic("Failed to map kernel");

    if (vmm_map_bootstrap_memory(current_directory) != 0)
        panic("Failed to map bootstrap memory");

    if (vmm_map_vga(current_directory) != 0)
        panic("Failed to map VGA");

    enable_paging((uint32_t)current_directory);
    recursive_mapping_ready = true;
}