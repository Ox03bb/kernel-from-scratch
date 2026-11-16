
#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utils/types.h"

#define THREAD_INVALID_ID UINT32_MAX

/* Thread states */
typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED,
    THREAD_INVALID
} thread_state_t;

/* CPU context */
typedef struct {
    registers_t registers;
    uint32_t eip;
    uint32_t cs; // hardcoded in this version to 0x08
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
extern uint32_t last_id;

/* Thread lifecycle */

thread_t *thread_create(const char *name, void (*entry)(void), size_t stack_size);

int8_t thread_destroy(thread_t *thread);

void thread_exit(void); //! not implimented

/* Thread state management */
int8_t thread_chstt(thread_t *thread, thread_state_t s);

int8_t thread_block(thread_t *thread);

int8_t thread_unblock(thread_t *thread);

void thread_yield(void); //! not implimented

/* Thread information */

uint32_t thread_get_id(thread_t *thread);

thread_state_t thread_get_state(thread_t *thread);

// important threads

thread_t *thread_create_bootstrap(void);
thread_t *thread_create_idle(size_t stack_size);

void thread_bootstrap(void);
void thread_idle(void);

#endif
