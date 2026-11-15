
#ifndef SCHED_H
#define SCHED_H

#include <stdbool.h>
#include <stdint.h>

#include "thread.h"

/* Scheduler structure */
typedef struct {
    thread_t *current_thread;
    thread_t *ready_queue;

    thread_t *idle_thread;

    uint32_t next_tid;
    uint32_t time_quantum;

    bool enabled;
} scheduler_t;

extern scheduler_t *sched;

/* Scheduler lifecycle */

void scheduler_init(void);

void scheduler_start(void);

void scheduler_shutdown(void);

/* Scheduling operations */

void scheduler_schedule(void);

void scheduler_yield(void);

void scheduler_tick(void);

/* Thread management */

void scheduler_add(thread_t *thread);

void scheduler_remove(thread_t *thread);

thread_t *scheduler_next(void);

/* Scheduler information */

thread_t *scheduler_current_thread(void);

bool scheduler_is_enabled(void);

uint32_t scheduler_get_tick_count(void);

#endif
