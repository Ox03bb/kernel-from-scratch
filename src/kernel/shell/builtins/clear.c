#include "shell/builtins/builtins.h"

int shell_builtin_clear(shell_context_t *context, int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    if (context != NULL && context->tty != NULL)
        context->tty->method.clear(context->tty);
    return 0;
}
