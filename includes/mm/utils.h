#ifndef VMM_UTILS_H
#define VMM_UTILS_H

#include "mm/pmm.h"
#include "mm/vmm.h"

int vmm_map_kernel(pde_t *directory);
int vmm_map_vga(pde_t *directory);

#endif