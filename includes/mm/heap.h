#ifndef HEAP_H
#define HEAP_H

#include "kernel.h"
#include "mm/pmm.h"
#include "types.h"

#define K_HEAP_START 0xD0000000
#define K_HEAP_MAX   0xE0000000

#define K_HEAP_INITIAL_SIZE K_HEAP_MAX - K_HEAP_START

#endif