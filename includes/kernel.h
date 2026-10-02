#ifndef KERNEL_H
#define KERNEL_H

#include "stdint.h"
#include "types.h"
typedef struct {
    uint32_t memory_map_address;
    uint32_t memory_map_count;
} boot_info_t;

extern boot_info_t boot_info;

void kernel_main(boot_info_t *boot_info);

#endif