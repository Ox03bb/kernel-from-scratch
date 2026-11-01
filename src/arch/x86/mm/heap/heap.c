#include "heap.h"

#include "mm/utils.h"
#include "types.h"
#include "vmm.h"

#include "panic.h"

#include "stdio.h"

typedef struct heap_block {
    size_t size;
    bool is_free;
    struct heap_block *next;
} heap_block_t;

typedef struct {
    uintptr_t start;
    uintptr_t end;
    uintptr_t max;
    heap_block_t *head;
} heap_t;

int init_heap(pde_t *directory, heap_t *heap) {
    if (directory == NULL || heap == NULL)
        return -1;

    uintptr_t start = K_HEAP_START;
    uintptr_t end = start + K_HEAP_INITIAL_SIZE;

    if (end > K_HEAP_MAX)
        return -1;

    // Map the initial heap pages.
    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        if (vmm_allocate(directory, addr) != 0)
            return -1;
    }

    // Initialize heap metadata.
    heap->start = start;
    heap->end = end;
    heap->max = K_HEAP_MAX;

    // Initialize the first free block.
    heap_block_t *first = (heap_block_t *)start;

    first->size = K_HEAP_INITIAL_SIZE - sizeof(heap_block_t);
    first->is_free = true;
    first->next = NULL;

    heap->head = first;

    return 0;
}
