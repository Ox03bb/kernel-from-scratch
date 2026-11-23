#include "thread.h"
#include "sched/utils.h"

#include "stdio.h"
#include "utils/utils.h"

#include "heap.h"

#include "context.h"
#include "panic.h"
#include "sched.h"
#include <stdarg.h>

uint32_t last_id = 1;

extern scheduler_t *sched;

void thread_bootstrap(void) {
    thread_t *current = sched_current_thread();

    switch (current->arg_count) {
    case 0:
        ((void (*)(void))current->entry)();
        break;
    case 1:
        ((void (*)(uintptr_t))current->entry)(current->args[0]);
        break;
    case 2:
        ((void (*)(uintptr_t, uintptr_t))current->entry)(current->args[0], current->args[1]);
        break;
    case 3:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t))current->entry)(
            current->args[0], current->args[1], current->args[2]);
        break;
    case 4:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t))current->entry)(
            current->args[0], current->args[1], current->args[2], current->args[3]);
        break;
    case 5:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))current->entry)(
            current->args[0], current->args[1], current->args[2], current->args[3],
            current->args[4]);
        break;
    case 6:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                   uintptr_t))current->entry)(current->args[0], current->args[1], current->args[2],
                                              current->args[3], current->args[4], current->args[5]);
        break;
    case 7:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                   uintptr_t))current->entry)(current->args[0], current->args[1], current->args[2],
                                              current->args[3], current->args[4], current->args[5],
                                              current->args[6]);
        break;
    case 8:
        ((void (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                   uintptr_t))current->entry)(current->args[0], current->args[1], current->args[2],
                                              current->args[3], current->args[4], current->args[5],
                                              current->args[6], current->args[7]);
        break;
    default:
        panic("Thread argument count exceeds supported limit");
    }

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
    thread->arg_count = 0;
    thread->saved_esp = 0;
    thread->saved_frame = NULL;
    for (uint32_t i = 0; i < THREAD_MAX_ARGS; i++)
        thread->args[i] = 0;
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
    thread->arg_count = 0;
    thread->saved_frame = NULL;
    for (uint32_t i = 0; i < THREAD_MAX_ARGS; i++)
        thread->args[i] = 0;
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
    t->arg_count = 0;
    t->saved_frame = NULL;
    for (uint32_t i = 0; i < THREAD_MAX_ARGS; i++)
        t->args[i] = 0;
    t->next = NULL;

    interrupt_frame_t *ctx = malloc(sizeof(interrupt_frame_t));

    if (ctx == NULL) {
        free(t->stack);
        free(t);
        return NULL;
    }

    /* This frame is disjoint from the thread stack. */
    uintptr_t stack_top = ((uintptr_t)t->stack + t->stack_size) & ~(uintptr_t)0xF;

    *ctx = (interrupt_frame_t){.registers = {.edi = 0,
                                             .esi = 0,
                                             .ebp = 0,
                                             .esp = (uint32_t)stack_top,
                                             .ebx = 0,
                                             .edx = 0,
                                             .ecx = 0,
                                             .eax = 0},

                               .vector = 0,
                               .eflags = 0x202,
                               .eip = (uint32_t)thread_bootstrap,
                               .cs = 0x08};
    t->saved_frame = ctx;
    t->saved_esp = (uintptr_t)ctx;

    return t;
}

thread_t *thread_spawn_impl(void (*entry)(void *), ...) {
    va_list args;
    va_start(args, entry);

    uint32_t arg_count = 0;
    uintptr_t values[THREAD_MAX_ARGS];

    while (arg_count < THREAD_MAX_ARGS) {
        uintptr_t value = va_arg(args, uintptr_t);
        if (value == UINTPTR_MAX)
            break;

        values[arg_count++] = value;
    }

    va_end(args);

    thread_t *thread = thread_create(NULL, (void (*)(void))entry, 4096);

    if (thread == NULL)
        return NULL;

    thread->arg_count = arg_count;
    for (uint32_t i = 0; i < arg_count; i++)
        thread->args[i] = values[i];

    if (sched_add(thread) != 0) {
        thread_destroy(thread);
        return NULL;
    }

    return thread;
}

int8_t thread_destroy(thread_t *thread) {
    if (thread == NULL)
        return -1;

    free(thread->saved_frame);
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