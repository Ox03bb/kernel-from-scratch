#include "mm/pmm.h"
#include "stdio.h"

#include "types.h"

#include "vga.h"
#include "vga_lib.h"

// in memory map 0 mean usable and 1 mean reserved

void memory_detect_verbose(memory_map_t *memory_map) {
    for (uint32_t i = 0; i < memory_map->count; i++) {
        memory_map_entry_t *entry = &memory_map->entries[i];

        uint64_t size_mb = entry->length / (1024 * 1024);

        printf("[\033[34m%d\033[0m] base=0x", (int)i);
        print_hex_p((uint32_t)(entry->base >> 32), 1);
        print_hex_p((uint32_t)entry->base, 8);
        printf(" length=0x");
        print_hex_p((uint32_t)(entry->length >> 32), 1);
        print_hex_p((uint32_t)entry->length, 8);
        printf(" type=%d ", entry->type);
        printf(" attributes=0x%x ", entry->attributes);
        printf(" size=%d MB\n", (int)size_mb);
    }
}

void pmm_memory_detect(memory_map_t *memory_map, boot_info_t *boot_info, bool verbose) {
    memory_map_entry_t *data = (memory_map_entry_t *)boot_info->memory_map_address;

    memory_map->count = boot_info->memory_map_count;
    memory_map->entries = data;

    print("[\033[34minit\033[0m] ");
    vga_print("Memory detection\n");

    if (verbose) {
        memory_detect_verbose(memory_map);
    } else {
        print_at_end("... Ok\n", GREEN);
    }
}

void pmm_memory_map(memory_map_t *mm, uint8_t *bitmap) {
    const uint64_t bitmap_frame_count = BITMAP_SIZE * 8ULL;

    for (uint32_t i = 0; i < mm->count; i++) {
        memory_map_entry_t *entry = &mm->entries[i];

        if (entry->type != TYPE_USABLE)
            continue;

        uint64_t first_frame = entry->base / FRAME_SIZE;
        uint64_t frame_count = entry->length / FRAME_SIZE;

        if (first_frame >= bitmap_frame_count)
            continue;

        if (frame_count > bitmap_frame_count - first_frame)
            frame_count = bitmap_frame_count - first_frame;

        uint64_t end_frame = first_frame + frame_count;

        while (first_frame < end_frame && first_frame % 8 != 0) {
            bitmap[first_frame / 8] &= (uint8_t)~(1U << (first_frame % 8));
            first_frame++;
        }

        while (first_frame + 8 <= end_frame) {
            bitmap[first_frame / 8] = 0;
            first_frame += 8;
        }

        while (first_frame < end_frame) {
            bitmap[first_frame / 8] &= (uint8_t)~(1U << (first_frame % 8));
            first_frame++;
        }
    }
}

void pmm_reserve(uint8_t *bitmap, uintptr_t start_addr, uintptr_t end_addr) {
    uint64_t first_frame = start_addr / FRAME_SIZE;
    uint64_t end_frame = (end_addr + FRAME_SIZE - 1) / FRAME_SIZE;

    for (uint64_t frame = first_frame; frame < end_frame; frame++) {
        bitmap[frame / 8] |= (uint8_t)(1U << (frame % 8));
    }
}

void pmm_reserve_frame(uint8_t *bitmap, uintptr_t start_addr, int frame_count) {
    uint64_t first_frame = start_addr / FRAME_SIZE;

    for (uint64_t i = 0; i < (uint64_t)frame_count; i++) {
        uint64_t frame = first_frame + i;

        bitmap[frame / 8] |= (uint8_t)(1U << (frame % 8));
    }
}

bool pmm_check_mm(uint8_t *bitmap, uint32_t addr) {
    uint64_t frame = addr / FRAME_SIZE;

    return (bitmap[frame / 8] & (1U << (frame % 8))) != 0;
}

bool pmm_check_mm_index(uint8_t *bitmap, uint32_t index) {
    return (bitmap[index / 8] & (1U << (index % 8))) != 0;
}

// helper

int find_n_usable_space(uint8_t *bitmap, int count) {
    int consecutive = 0;

    for (int i = 0; i < BITMAP_SIZE; i++) {
        uint8_t byte = bitmap[i];

        for (int bit = 0; bit < 8; bit++) {
            if ((byte & (1U << bit)) == 0) {
                consecutive++;

                if (consecutive == count)
                    return (i * 8) + bit - count + 1;
            } else {
                consecutive = 0;
            }
        }
    }

    return -1;
}

// alloc
uintptr_t pmm_alloc_frame(uint8_t *bitmap) {
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        if (bitmap[i] != 0xFF) {
            uint8_t n = __builtin_ctz((uint8_t)~bitmap[i]);

            bitmap[i] |= (uint8_t)(1U << n);

            return ((i * 8) + n) * FRAME_SIZE;
        }
    }

    return 0;
}

uintptr_t pmm_alloc_n_frame(uint8_t *bitmap, int count) {
    int frame = find_n_usable_space(bitmap, count);

    if (frame < 0)
        return 0;

    for (int i = 0; i < count; i++) {
        int current_frame = frame + i;

        bitmap[current_frame / 8] |= (uint8_t)(1U << (current_frame % 8));
    }

    return (uintptr_t)frame * FRAME_SIZE;
}

// free

void pmm_free_frame(uint8_t *bitmap, uintptr_t addr) {
    size_t frame = addr / FRAME_SIZE;

    bitmap[frame / 8] &= (uint8_t)~(1U << (frame % 8));
}

void pmm_free_n_frame(uint8_t *bitmap, uintptr_t address, int count) {
    int frame = address / FRAME_SIZE;

    for (int i = 0; i < count; i++) {
        int current_frame = frame + i;

        bitmap[current_frame / 8] &= (uint8_t)~(1U << (current_frame % 8));
    }
}