#ifndef CONTEXT_H
#define CONTEXT_H

#include "thread.h"
#include "utils/utils.h"

void ctx_init(thread_t *thread);

cpu_context_t ctx_current();

void ctx_save(registers_t *ctx);
void ctx_load(registers_t *ctx);

#endif