#include "context.h"
#include "thread.h"

#include "utils/utils.h"

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

void ctx_switch(cpu_context_t *old_ctx, cpu_context_t *new_ctx) {}
