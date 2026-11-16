#ifndef CONTEXT_H
#define CONTEXT_H

#include "thread.h"

void ctx_init(thread_t *thread);
cpu_context_t ctx_current();

#endif