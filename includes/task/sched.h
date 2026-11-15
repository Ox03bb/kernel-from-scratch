
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

/* Scheduler lifecycle */

void sched_init(void);

void sched_start(void);

void sched_shutdown(void);

/* Scheduling operations */

void sched_schedule(void);

void sched_yield(void);

void sched_tick(void);

/* Thread management */

void sched_add(thread_t *thread);

void sched_remove(thread_t *thread);

thread_t *sched_next(void);

/* Scheduler information */

thread_t *sched_current_thread(void);

bool sched_is_enabled(void);

uint32_t sched_get_tick_count(void);

#endif
