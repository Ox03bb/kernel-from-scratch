#include "fs.h"
#include "shell/builtins/builtins.h"

int shell_builtin_cat(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];
    char buffer[256];
    fs_node_t *node;
    size_t offset;

    if (argc < 2) {
        shell_builtin_write(context, "cat: missing operand\n");
        return -1;
    }

    shell_resolve_path(context, argv[1], resolved, sizeof(resolved));
    node = fs_lookup(resolved);
    if (node == NULL || node->type != FS_FILE) {
        shell_builtin_write(context, "cat: not a file\n");
        return -1;
    }

    offset = 0;
    while (offset < node->size) {
        size_t to_read = node->size - offset;
        if (to_read >= sizeof(buffer))
            to_read = sizeof(buffer) - 1;

        int count = fs_read(node, buffer, to_read, offset);
        if (count <= 0)
            break;
        buffer[count] = '\0';
        shell_builtin_write(context, buffer);
        offset += (size_t)count;
    }
    return 0;
}
