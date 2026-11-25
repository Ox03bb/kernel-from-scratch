#include "fs.h"
#include "shell/builtins/builtins.h"
#include "stdio.h"
#include "string.h"

#define TREE_MAX_DEPTH 16

static void tree_print_node(shell_context_t *context, fs_node_t *node, int depth, bool is_last) {
    (void)context;
    (void)is_last;

    if (node == NULL || depth > TREE_MAX_DEPTH)
        return;

    for (int i = 0; i < depth; i++)
        printf("    ");

    if (depth > 0)
        printf(is_last ? "`-- " : "|-- ");

    if (node->type == FS_DIR)
        printf("\033[14:00m%s\033[0m", node->name);
    else
        printf("%s", node->name);
    printf("\n");

    if (node->type == FS_DIR) {
        fs_node_t *child = node->children;
        while (child != NULL) {
            bool child_is_last = (child->next == NULL);
            tree_print_node(context, child, depth + 1, child_is_last);
            child = child->next;
        }
    }
}

int shell_builtin_tree(shell_context_t *context, int argc, char *argv[]) {
    char resolved[SHELL_PATH_SIZE];
    fs_node_t *node;

    (void)argc;

    if (argv != NULL && argv[1] != NULL && argv[1][0] != '\0')
        shell_resolve_path(context, argv[1], resolved, sizeof(resolved));
    else
        shell_resolve_path(context, context->currentDir, resolved, sizeof(resolved));

    node = fs_lookup(resolved);
    if (node == NULL) {
        printf("tree: no such directory or file\n");
        return -1;
    }

    if (node->type == FS_DIR)
        printf("-- \033[14:00m%s\033[0m", node->name);
    else
        printf("-- %s", node->name);
    printf("\n");

    if (node->type == FS_DIR) {
        fs_node_t *child = node->children;
        while (child != NULL) {
            bool child_is_last = (child->next == NULL);
            tree_print_node(context, child, 1, child_is_last);
            child = child->next;
        }
    }

    return 0;
}
