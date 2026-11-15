#include "thread.h"
#include "task/utils.h"

#include "heap.h"
#include "sched.h"

thread_t *thread_create(
    const char *name,
    void (*entry)(void),
    size_t stack_size
) {
    if (!entry || stack_size < sizeof(cpu_context_t))
        return NULL;

    thread_t *t = malloc(sizeof(thread_t));
    if (!t)
        return NULL;

    t->stack = malloc(stack_size);
    if (!t->stack) {
        free(t);
        return NULL;
    }

    t->tid = ++last_id;
    t->name = name;
    t->state = THREAD_READY;

    t->stack_size = stack_size;
    t->entry = entry;
    t->next = NULL;

    // Calculate aligned stack top.
    uintptr_t stack_top =
        ((uintptr_t)t->stack + stack_size) & ~(uintptr_t)0xF;

    // Reserve space for the initial CPU context.
    stack_top -= sizeof(cpu_context_t);

    // Initialize the initial context.
    cpu_context_t *ctx = (cpu_context_t *)stack_top;

    *ctx = (cpu_context_t) {
        .edi = 0,
        .esi = 0,
        .ebp = 0,
        .esp_dummy = 0,
        .ebx = 0,
        .edx = 0,
        .ecx = 0,
        .eax = 0,

        .eip = (uint32_t)thread_bootstrap,
        .cs = 0x08,
        .eflags = 0x202
    };

    t->saved_esp = (uint32_t)ctx;

    return t;
}