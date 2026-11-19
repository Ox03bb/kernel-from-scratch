#ifndef CONTEXT_H
#define CONTEXT_H

#include "thread.h"
#include "utils/utils.h"

void ctx_init(thread_t *thread);

cpu_context_t ctx_current();

uint32_t ctx_save(void);
void ctx_load(uint32_t esp);

void ctx_switch(thread_t *old_t, thread_t *new_t);

#endif