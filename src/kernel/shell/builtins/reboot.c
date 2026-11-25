#include "shell/builtins/builtins.h"

#include "utils/utils.h"

/* Trigger a CPU reset by faulting with an empty IDT. */
static void reboot_triple_fault(void) {
    uint8_t null_idtr[6] = {0}; /* limit = 0, base = 0 */

    cli();
    __asm__ volatile("lidt %0" :: "m"(null_idtr));
    __asm__ volatile("int3");
    while (1) {
        hlt();
    }
}

int shell_builtin_reboot(shell_context_t *context, int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    shell_builtin_write(context, "Rebooting...\n");
    reboot_triple_fault();
    return 0; /* unreachable */
}
