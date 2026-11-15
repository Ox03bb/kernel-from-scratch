#include "task/utils.h"

bool sq_is_empty(const scheduler_t *scheduler) {
    return scheduler == NULL || scheduler->ready_queue == NULL;
}

void sq_push(scheduler_t *scheduler, thread_t *thread) {
    if (scheduler == NULL || thread == NULL)
        return;

    thread->next = NULL;

    if (scheduler->ready_queue == NULL) {
        scheduler->ready_queue = thread;
        return;
    }

    thread_t *tail = scheduler->ready_queue;
    while (tail->next != NULL)
        tail = tail->next;

    tail->next = thread;
}

thread_t *sq_pop(scheduler_t *scheduler) {
    if (scheduler == NULL || scheduler->ready_queue == NULL)
        return NULL;

    thread_t *thread = scheduler->ready_queue;
    scheduler->ready_queue = thread->next;
    thread->next = NULL;

    return thread;
}

bool sq_remove(scheduler_t *scheduler, thread_t *thread) {
    if (scheduler == NULL || thread == NULL)
        return false;

    thread_t **current = &scheduler->ready_queue;
    while (*current != NULL) {
        if (*current == thread) {
            *current = thread->next;
            thread->next = NULL;
            return true;
        }

        current = &(*current)->next;
    }

    return false;
}

thread_t *sq_peek(const scheduler_t *scheduler) {
    if (scheduler == NULL)
        return NULL;

    return scheduler->ready_queue;
}

bool sq_contains(const scheduler_t *scheduler, const thread_t *thread) {
    if (scheduler == NULL || thread == NULL)
        return false;

    for (const thread_t *current = scheduler->ready_queue;
         current != NULL;
         current = current->next) {
        if (current == thread)
            return true;
    }

    return false;
}
