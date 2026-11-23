
#ifndef sched_utils_H
#define sched_utils_H

#include "sched.h"
#include <stdbool.h>

/* Scheduler ready-queue operations */

bool sq_is_empty(const scheduler_t *scheduler);

void sq_push(scheduler_t *scheduler, thread_t *thread);

thread_t *sq_pop(scheduler_t *scheduler);

bool sq_remove(scheduler_t *scheduler, thread_t *thread);

thread_t *sq_peek(const scheduler_t *scheduler);

bool sq_contains(const scheduler_t *scheduler, const thread_t *thread);

#endif
