#include "context.h"
#include "thread.h"

#include "utils/utils.h"

//! for this version will not use the registers_t
//! to change ctx, w'll use the thread stack directly

void ctx_init(thread_t *thread) {
    uintptr_t stack_top = (uintptr_t)thread->stack + thread->stack_size;

    stack_top &= ~(uintptr_t)0xF;

    stack_top -= sizeof(cpu_context_t);

    cpu_context_t *ctx = (cpu_context_t *)stack_top;

    *ctx = (cpu_context_t){
        .registers = {0}, .eip = (uint32_t)thread_bootstrap, .cs = 0x08, .eflags = 0x202};

    thread->saved_esp = (uint32_t)ctx;
}

cpu_context_t ctx_current() {

    registers_t r = pushad_r();
    uint32_t flg = pushfd_r();
    uint32_t epi = reg_v(REG_EIP);

    cpu_context_t ctx = {
        .registers = r,
        .eip = epi,
        .cs = 0x08, // hardcoed for this version
        .eflags = flg,
    };

    return ctx;
}

void ctx_switch(thread_t *old_t, thread_t *new_t) {

    if (!new_t)
        return;

    if (old_t)
        old_t->saved_esp = ctx_save();

    ctx_load(new_t->saved_esp);
}