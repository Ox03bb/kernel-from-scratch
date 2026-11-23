#include "shell/builtins/builtins.h"

int shell_builtin_help(shell_context_t *context, int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    shell_builtin_write(context, "Built-in commands:\n");
    shell_builtin_write(context, "  help     Show this list\n");
    shell_builtin_write(context, "  clear    Clear the screen\n");
    shell_builtin_write(context, "  echo     Print text\n");
    shell_builtin_write(context, "  pwd      Print working directory\n");
    shell_builtin_write(context, "  ls       List directory contents\n");
    shell_builtin_write(context, "  cd       Change directory\n");
    shell_builtin_write(context, "  mkdir    Create a directory\n");
    shell_builtin_write(context, "  touch    Create a file\n");
    shell_builtin_write(context, "  rm       Remove a file or empty directory\n");
    shell_builtin_write(context, "  cat      Print a file\n");
    return 0;
}
