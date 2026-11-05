#ifndef HEAP_H
#define HEAP_H

#include "kernel.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "types.h"

#define K_HEAP_START 0xD0000000
#define K_HEAP_MAX   0xE0000000

#define K_HEAP_INITIAL_SIZE K_HEAP_MAX - K_HEAP_START

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

int8_t init_heap(pde_t *directory, heap_t *heap);

void *gmalloc(heap_t *heap, size_t size);
int8_t gfree(heap_t *heap, void *ptr);

// void *kmalloc(size_t size);
// void kfree(void *ptr);

#endif