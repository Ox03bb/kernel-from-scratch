#include "fs.h"
#include "shell/builtins/builtins.h"

int shell_builtin_cd(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];
    fs_node_t *node;

    shell_resolve_path(context, argc > 1 ? argv[1] : "/", resolved, sizeof(resolved));
    node = fs_lookup(resolved);
    if (node == NULL || node->type != FS_DIR) {
        shell_builtin_write(context, "cd: no such directory\n");
        return -1;
    }

    size_t i;
    for (i = 0; resolved[i] != '\0' && i < SHELL_PATH_SIZE - 1; i++)
        context->currentDir[i] = resolved[i];
    context->currentDir[i] = '\0';
    return 0;
}
