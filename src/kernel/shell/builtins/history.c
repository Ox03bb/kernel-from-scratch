#include "shell/builtins/builtins.h"

#include "stdio.h"

int shell_builtin_history(shell_context_t *context, int argc, char *argv[]) {
    char line[SHELL_LINE_SIZE + 16];

    (void)argc;
    (void)argv;

    if (context == NULL)
        return -1;

    shell_builtin_write(context, "Command history:\n");

    for (size_t i = 0; i < context->history_count; i++) {
        snprintf(line, sizeof(line), "  %zu  %s\n", i + 1, context->history[i]);
        shell_builtin_write(context, line);
    }

    if (context->history_count == 0)
        shell_builtin_write(context, "  (empty)\n");

    return 0;
}
