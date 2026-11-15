
#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Thread states */
typedef enum { THREAD_READY, THREAD_RUNNING, THREAD_BLOCKED, THREAD_TERMINATED } thread_state_t;

/* CPU context */
typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} cpu_context_t;

/* Thread structure */
typedef struct thread {
    uint32_t tid;
    const char *name;

    thread_state_t state;

    uint32_t saved_esp;

    void *stack;
    size_t stack_size;

    void (*entry)(void);

    struct thread *next;
} thread_t;
extern uint32_t last_id = 1;

/* Thread lifecycle */

thread_t *thread_create(const char *name, void (*entry)(void), size_t stack_size);

void thread_destroy(thread_t *thread);

void thread_exit(void);

/* Thread state management */

void thread_block(thread_t *thread);

void thread_unblock(thread_t *thread);

void thread_yield(void);

/* Thread information */

thread_t *thread_current(void);

uint32_t thread_get_id(thread_t *thread);

thread_state_t thread_get_state(thread_t *thread);

#endif
