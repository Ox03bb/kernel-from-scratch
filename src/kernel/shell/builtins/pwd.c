#include "shell/builtins/builtins.h"

int shell_builtin_pwd(shell_context_t *context, int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    shell_builtin_write(context, context->currentDir);
    shell_builtin_write(context, "\n");
    return 0;
}
