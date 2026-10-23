#ifndef VMM_H
#define VMM_H

#include "kernel.h"
#include "mm/pmm.h"
#include "types.h"

#define PAGE_ENTRIES 1024
#define PAGE_SIZE    FRAME_SIZE

typedef union {
    uint32_t raw;

    struct {
        uint32_t p : 1;     // Present
        uint32_t rw : 1;    // Read/Write
        uint32_t us : 1;    // User/Supervisor
        uint32_t pwt : 1;   // Write-Through
        uint32_t pcd : 1;   // Cache Disable
        uint32_t a : 1;     // Accessed
        uint32_t d : 1;     // Dirty
        uint32_t pat : 1;   // PAT
        uint32_t g : 1;     // Global
        uint32_t avl : 3;   // Available
        uint32_t addr : 20; // Physical frame address
    };
} pte_t; // page table entry

// page directory
typedef union {
    uint32_t raw;

    struct {
        uint32_t p : 1;     // Present
        uint32_t rw : 1;    // Read/Write
        uint32_t us : 1;    // User/Supervisor
        uint32_t pwt : 1;   // Write-Through
        uint32_t pcd : 1;   // Cache Disable
        uint32_t a : 1;     // Accessed
        uint32_t ign : 1;   // Ignored
        uint32_t ps : 1;    // Page Size
        uint32_t g : 1;     // Global
        uint32_t avl : 3;   // Available
        uint32_t addr : 20; // Physical frame address
    };
} pde_t; // page directory entry

// virual address

typedef union {
    uint32_t raw;

    struct {
        uint32_t offset : 12; // Offset within the page
        uint32_t pt : 10;     // Page table index
        uint32_t pd : 10;     // Page directory index
    };
} vaddr_t; // virtual address

void enable_paging(uint32_t page_dir);

pde_t *vmm_create_directory();
pte_t *vmm_create_page_table();

int8_t vmm_map_page(pde_t *directory, uint32_t virtual_addr, uint32_t physical_addr,
                    uint32_t flags);

int8_t vmm_unmap_page(pde_t *directory, uint32_t virtual_addr);

pte_t *vmm_get_pte(pde_t *directory, vaddr_t va);

void vmm_invalidate_page(uint32_t virtual_addr);

uintptr_t vmm_get_phys_addr(pde_t *directory, vaddr_t va, bool *err);

void load_directory(uint32_t page_dir);
void enable_paging(uint32_t page_dir);

void vmm_page_fault_handler(); // mm/page_fault.c

void vmm_init();

#endif