#include "thread.h"
#include "task/utils.h"

#include "stdio.h"
#include "utils/utils.h"

#include "heap.h"

#include "context.h"
#include "sched.h"

uint32_t last_id = 1;

void thread_bootstrap(void) {
    thread_t *current = sched_current_thread();

    current->entry();

    thread_exit();
}

void thread_idle(void) {
    while (1) {
        sti();
        hlt();
    }
}

thread_t *thread_create_bootstrap(void) {
    thread_t *thread = malloc(sizeof(thread_t));

    if (thread == NULL)
        return NULL;

    thread->tid = 0;
    thread->name = "bootstrap";
    thread->state = THREAD_RUNNING;

    thread->stack = NULL;
    thread->stack_size = 0;

    thread->entry = NULL;
    thread->next = NULL;

    return thread;
}

thread_t *thread_create_idle(size_t stack_size) {
    thread_t *thread = malloc(sizeof(thread_t));

    if (thread == NULL)
        return NULL;

    thread->stack = malloc(stack_size);

    if (thread->stack == NULL) {
        free(thread);
        return NULL;
    }

    thread->tid = 1;
    thread->name = "idle";
    thread->state = THREAD_READY;

    thread->stack_size = stack_size;
    thread->entry = thread_idle;
    thread->next = NULL;

    // Initialize the thread's initial CPU context.
    ctx_init(thread);

    return thread;
}

thread_t *thread_create(const char *name, void (*entry)(void), size_t stack_size) {
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
    uintptr_t stack_top = ((uintptr_t)t->stack + stack_size) & ~(uintptr_t)0xF;

    // Reserve space for the initial CPU context.
    stack_top -= sizeof(cpu_context_t);

    // Initialize the initial context.
    cpu_context_t *ctx = (cpu_context_t *)stack_top;

    *ctx = (cpu_context_t){
        .registers = {0}, .eip = (uint32_t)thread_bootstrap, .cs = 0x08, .eflags = 0x202};

    t->saved_esp = (uint32_t)ctx;

    return t;
}

int8_t thread_destroy(thread_t *thread) {
    if (thread == NULL)
        return -1;

    free(thread->stack);
    free(thread);

    return 0;
}

int8_t thread_chstt(thread_t *thread, thread_state_t s) {
    if (thread == NULL)
        return -1;

    thread->state = s;

    return 0;
}

int8_t thread_block(thread_t *thread) {
    if (thread == NULL)
        return -1;

    thread->state = THREAD_BLOCKED;

    return 0;
}

int8_t thread_unblock(thread_t *thread) {
    if (thread == NULL)
        return -1;

    thread->state = THREAD_READY;

    return 0;
}

uint32_t thread_get_id(thread_t *thread) {
    if (thread == NULL)
        return THREAD_INVALID_ID;

    return thread->tid;
}

thread_state_t thread_get_state(thread_t *thread) {
    if (thread == NULL)
        return THREAD_INVALID;

    return thread->state;
}

void thread_exit(void) {
    cli();
    thread_t *t = sched_current_thread();

    t->state = THREAD_TERMINATED;

    sched_remove(t);

    // sched_schedule();
}