#include "fs.h"
#include "shell/builtins/builtins.h"
#include "stdio.h"

int shell_builtin_ls(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];
    fs_node_t *node;

    if (argc > 1)
        shell_resolve_path(context, argv[1], resolved, sizeof(resolved));
    else
        shell_resolve_path(context, context->currentDir, resolved, sizeof(resolved));

    node = fs_lookup(resolved);
    if (node == NULL || node->type != FS_DIR) {
        shell_builtin_write(context, "ls: not a directory\n");
        return -1;
    }

    fs_node_t *child = node->children;
    bool first = true;
    while (child != NULL) {
        if (!first)
            printf("  ");
        if (child->type == FS_DIR)
            printf("\033[14:00m%s\033[0m", child->name);
        else
            printf("%s", child->name);
        first = false;
        child = child->next;
    }
    printf("\n");
    return 0;
}
