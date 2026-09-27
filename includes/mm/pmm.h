#ifndef PMM_H
#define PMM_H

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

#endif