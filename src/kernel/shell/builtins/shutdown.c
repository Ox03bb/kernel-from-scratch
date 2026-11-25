#include "shell/builtins/builtins.h"

#include "utils/utils.h"

int shell_builtin_shutdown(shell_context_t *context, int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    shell_builtin_write(context, "Shutting down... (CPU halted)\n");
    cli();
    while (1) {
        hlt();
    }
    return 0; /* unreachable */
}
