#include "sched.h"
#include "heap.h"
#include "task/utils.h"
#include "thread.h"
#include "utils/utils.h"

extern scheduler_t *sched;

void sched_init(void) {
    cli();

    sched = malloc(sizeof(scheduler_t));

    sched->enabled = false;

    sched->time_quantum = 10;
    sched->remaining_ticks = sched->time_quantum;
    sched->need_resched = false;
    sched->ready_queue = NULL;
    sched->next_tid = 0;

    sched->current_thread = thread_create_bootstrap();

    sched->idle_thread = thread_create_idle(4096);
}

void sched_start(void) {
    if (sched == NULL)
        return;

    sched->enabled = true;
    sti();
}

void sched_shutdown(void) {
    if (sched == NULL)
        return;

    sched->enabled = false;
}

void sched_tick(void) {
    if (!sched->enabled)
        return;

    thread_t *current = sched->current_thread;

    if (current == NULL)
        return;

    if (current == sched->idle_thread) {
        if (!sq_is_empty(sched))
            sched->need_resched = true;

        return;
    }

    if (current->state != THREAD_RUNNING)
        return;

    if (--sched->remaining_ticks == 0)
        sched->need_resched = true;
}

int8_t sched_add(thread_t *thread) {
    if (thread == NULL)
        return -1;

    if (thread->state != THREAD_READY)
        return -1;

    if (thread == sched->idle_thread)
        return -1;

    sq_push(sched, thread);

    return 0;
}

int8_t sched_remove(thread_t *thread) {
    if (thread == NULL)
        return -1;

    if (thread == NULL)
        return -1;

    if (!sq_contains(sched, thread))
        return -1;

    sq_remove(sched, thread);

    return 0;
}

thread_t *sched_next(void) {
    thread_t *next = sq_pop(sched);

    if (next == NULL)
        return sched->idle_thread;

    return next;
}

thread_t *sched_current_thread(void) { return sched->current_thread; }
bool sched_is_enabled(void) { return sched->enabled; }
uint32_t sched_get_remaining_ticks(void) { return sched->remaining_ticks; }
