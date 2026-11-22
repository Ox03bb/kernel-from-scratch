#include "thread.h"
#include "task/utils.h"

#include "stdio.h"
#include "utils/utils.h"

#include "heap.h"

#include "context.h"
#include "sched.h"

uint32_t last_id = 1;

extern scheduler_t *sched;

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

void sched_schedule(void) {
    thread_t *current = sched->current_thread;

    // Requeue the current thread if it is still runnable.
    if (current->state == THREAD_RUNNING) {
        current->state = THREAD_READY;

        if (current != sched->idle_thread && current != sched->bootstrap_thread)
            sched_add(current);
    }

    // Find the next runnable thread.
    thread_t *next = NULL;

    while (!sq_is_empty(sched)) {
        next = sched_next();

        if (next->state == THREAD_READY)
            break;

        next = NULL;
    }

    if (next == NULL)
        next = sched->idle_thread;

    next->state = THREAD_RUNNING;

    sched->current_thread = next;

    if (current != next)
        ctx_switch(current, next);
}

thread_t *thread_create(const char *name, void (*entry)(void), size_t stack_size) {
    if (!entry || stack_size < 4096)
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

    // Align stack top to 16 bytes.
    uintptr_t stack_top = ((uintptr_t)t->stack + stack_size) & ~(uintptr_t)0xF;

    // Reserve initial interrupt context.
    stack_top -= sizeof(interrupt_frame_t);

    interrupt_frame_t *ctx = (interrupt_frame_t *)stack_top;

    *ctx = (interrupt_frame_t){
        .registers =
            {.edi = 0, .esi = 0, .ebp = 0, .esp = 0, .ebx = 0, .edx = 0, .ecx = 0, .eax = 0},

        .vector = 0,
        .eflags = 0x202,
        .eip = (uint32_t)thread_bootstrap,
        .cs = 0x08};

    t->saved_esp = (uintptr_t)ctx;

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

_Noreturn void thread_exit(void) {
    thread_t *current = sched_current_thread();

    current->state = THREAD_TERMINATED;

    sched_schedule();

    __builtin_unreachable();
}