#include "mm/pmm.h"
#include "stdio.h"

#include "vga.h"
#include "vga_lib.h"

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
