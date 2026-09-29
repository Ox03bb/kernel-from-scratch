#ifndef PMM_H
#define PMM_H

#include "kernel.h"
#include "types.h"

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) memory_map_entry_t;

typedef struct {
    uint32_t count;
    memory_map_entry_t *entries;
} memory_map_t;

void pmm_memory_detect(memory_map_t *memory_map, boot_info_t *boot_info, bool verbose);
#endif