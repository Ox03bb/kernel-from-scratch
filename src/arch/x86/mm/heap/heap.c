#include "heap.h"

#include "mm/utils.h"
#include "types.h"
#include "vmm.h"

#include "panic.h"

#include "stdio.h"

int8_t init_heap(pde_t *directory, heap_t *heap) {
    printf("cc\n");
    if (directory == NULL) {
        print("null");
        return -1;
    }

    uintptr_t start = K_HEAP_START;
    uintptr_t end = start + K_HEAP_INITIAL_SIZE;

    if (end > K_HEAP_MAX)
        return -1;

    printf("cc\n");

    print("\ninit heap\n");
    // Map the initial heap pages.
    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        print("a ");
        if (vmm_allocate(addr) != 0) {
            print("\n not a");
            return -1;
        }
    }

    printf("ff\n");

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

heap_block_t *_get_last(heap_t *heap) {
    if (heap == NULL || heap->head == NULL)
        return NULL;

    heap_block_t *hh = heap->head;

    while (hh->next != NULL)
        hh = hh->next;

    return hh;
}

heap_block_t *_get_first_fit(heap_t *heap, size_t size) {
    if (heap == NULL || heap->head == NULL)
        return NULL;

    heap_block_t *hh = heap->head;

    while (hh != NULL) {
        if (hh->is_free && hh->size >= size)
            return hh;

        hh = hh->next;
    }

    return NULL;
}

void *gmalloc(heap_t *heap, size_t size) {
    if (heap == NULL || size == 0)
        return NULL;

    heap_block_t *block = _get_first_fit(heap, size);

    if (block == NULL)
        return NULL;

    size_t remaining = block->size - size;

    block->is_free = false;

    if (remaining > sizeof(heap_block_t)) {
        heap_block_t *block_n = (heap_block_t *)((uintptr_t)(block + 1) + size);

        block_n->size = remaining - sizeof(heap_block_t);
        block_n->is_free = true;
        block_n->next = block->next;

        block->next = block_n;
        block->size = size;
    }

    return (void *)(block + 1);
}
int8_t gfree(heap_t *heap, void *ptr) {
    if (heap == NULL || ptr == NULL)
        return -1;

    heap_block_t *block = (heap_block_t *)(ptr)-1;

    block->is_free = true;

    return 0;
}