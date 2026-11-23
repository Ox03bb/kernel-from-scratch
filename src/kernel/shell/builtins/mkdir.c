#include "fs.h"
#include "shell/builtins/builtins.h"

int shell_builtin_mkdir(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];

    if (argc < 2) {
        shell_builtin_write(context, "mkdir: missing operand\n");
        return -1;
    }

    shell_resolve_path(context, argv[1], resolved, sizeof(resolved));
    if (fs_mkdir(resolved) < 0) {
        shell_builtin_write(context, "mkdir: cannot create directory\n");
        return -1;
    }
    return 0;
}
