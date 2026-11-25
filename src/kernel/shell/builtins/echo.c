#include "shell/builtins/builtins.h"

int shell_builtin_echo(shell_context_t *context, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (i > 1)
            shell_builtin_write(context, " ");
        shell_builtin_write(context, argv[i]);
    }
    shell_builtin_write(context, "\n");
    return 0;
}
