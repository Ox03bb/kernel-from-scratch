#ifndef PMM_H
#define PMM_H

#include "kernel.h"
#include "types.h"

#define FRAME_SIZE      4096
#define MAX_MEMORY_SIZE 0x100000000ULL // 4GB
#define MAX_PAGES       (MAX_MEMORY_SIZE / FRAME_SIZE)
#define BITMAP_SIZE     (MAX_PAGES / 8)

#define TYPE_USABLE   0x01
#define TYPE_RESERVED 0x02

// uint8_t bitmap[BITMAP_SImakeZE];

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
void pmm_memory_map(memory_map_t *memory_map, uint8_t *bitmap);

void pmm_reserve(uint8_t *bitmap, uintptr_t start_addr, uintptr_t end_addr);
void pmm_reserve_frame(uint8_t *bitmap, uintptr_t start_addr, int frame_count);

bool pmm_check_mm(uint8_t *bitmap, uint32_t addr);
bool pmm_check_mm_index(uint8_t *bitmap, uint32_t index);

uintptr_t pmm_alloc_frame(uint8_t *bitmap);
uintptr_t pmm_alloc_n_frame(uint8_t *bitmap, int count);

void pmm_free_frame(uint8_t *bitmap, uintptr_t address);
void pmm_free_n_frame(uint8_t *bitmap, uintptr_t address, int count);

#endif