#include "fs.h"
#include "shell/builtins/builtins.h"

int shell_builtin_rm(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];

    if (argc < 2) {
        shell_builtin_write(context, "rm: missing operand\n");
        return -1;
    }

    shell_resolve_path(context, argv[1], resolved, sizeof(resolved));
    if (fs_unlink(resolved) < 0) {
        shell_builtin_write(context, "rm: cannot remove\n");
        return -1;
    }
    return 0;
}
