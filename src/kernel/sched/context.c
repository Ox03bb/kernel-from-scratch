#include "context.h"
#include "heap.h"
#include "thread.h"

#include "utils/utils.h"

extern interrupt_frame_t *active_irq_frame;

//! for this version will not use the registers_t
//! to change ctx, w'll use the thread stack directly

void ctx_init(thread_t *thread) {
    uintptr_t stack_top = (uintptr_t)thread->stack + thread->stack_size;
    stack_top &= ~(uintptr_t)0xF;

    interrupt_frame_t *ctx = malloc(sizeof(interrupt_frame_t));

    if (ctx == NULL)
        return;

    *ctx = (interrupt_frame_t){.registers = {.edi = 0,
                                             .esi = 0,
                                             .ebp = 0,
                                             .esp = (uint32_t)stack_top,
                                             .ebx = 0,
                                             .edx = 0,
                                             .ecx = 0,
                                             .eax = 0},
                               .vector = 0,
                               .eip = (uint32_t)thread_idle,
                               .cs = 0x08,
                               .eflags = 0x202};
    thread->saved_frame = ctx;
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

    if (old_t && active_irq_frame) {
        if (old_t->saved_frame == NULL)
            old_t->saved_frame = malloc(sizeof(interrupt_frame_t));

        if (old_t->saved_frame != NULL) {
            old_t->saved_esp = (uint32_t)old_t->saved_frame;
            uint32_t *destination = (uint32_t *)old_t->saved_frame;
            uint32_t *source = (uint32_t *)active_irq_frame;
            for (uint32_t i = 0; i < sizeof(interrupt_frame_t) / sizeof(uint32_t); i++)
                destination[i] = source[i];

            /*
             * PUSHA saved ESP points at the IRQ-vector slot, not at the
             * pre-interrupt ESP restored by IRET.
             */
            old_t->saved_frame->registers.esp = destination[3] + 16;
        }
    }

    ctx_load(new_t->saved_esp);
}