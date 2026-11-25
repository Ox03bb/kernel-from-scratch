#include "shell/builtins/builtins.h"

#include "stdio.h"
#include "timer.h"

int shell_builtin_info(shell_context_t *context, int argc, char *argv[]) {
    char buffer[32];

    (void)argc;
    (void)argv;

    shell_builtin_write(context, "kernel-from-scratch\n");
    shell_builtin_write(context, "  uptime: ");
    snprintf(buffer, sizeof(buffer), "%u seconds\n", timer_get_seconds());
    shell_builtin_write(context, buffer);

    return 0;
}
