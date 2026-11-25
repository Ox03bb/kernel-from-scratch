#include "fs.h"
#include "shell/builtins/builtins.h"

#include "lib/stdio.h"
#include "string.h"

#define CP_BUF_SIZE 256

/* Return the basename (last path component) of path. */
static const char *basename_of(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

/* Build the actual destination: if dst is a directory, place src's basename
 * inside it (cp file dir -> dir/file); otherwise use dst verbatim. */
static void resolve_actual_dst(const char *src_path, const char *dst_path, char *actual_dst,
                               size_t actual_size) {
    fs_node_t *dst_node = fs_lookup(dst_path);
    const char *base;

    if (dst_node != NULL && dst_node->type == FS_DIR) {
        base = basename_of(src_path);
        snprintf(actual_dst, actual_size, "%s/%s", dst_path, base);
    } else {
        snprintf(actual_dst, actual_size, "%s", dst_path);
    }
}

static int copy_file(shell_context_t *context, const char *src_path, const char *dst_path) {
    fs_node_t *src;
    fs_node_t *dst;
    char actual_dst[SHELL_PATH_SIZE];
    char buffer[CP_BUF_SIZE];
    size_t offset;
    int count;

    src = fs_lookup(src_path);
    if (src == NULL || src->type != FS_FILE) {
        shell_builtin_write(context, "cp: source not a file\n");
        return -1;
    }

    resolve_actual_dst(src_path, dst_path, actual_dst, sizeof(actual_dst));

    /* Overwrite an existing regular file destination. */
    if (fs_lookup(actual_dst) != NULL) {
        if (fs_unlink(actual_dst) < 0) {
            shell_builtin_write(context, "cp: cannot overwrite destination\n");
            return -1;
        }
    }

    if (fs_create(actual_dst) < 0) {
        shell_builtin_write(context, "cp: cannot create destination\n");
        return -1;
    }

    dst = fs_lookup(actual_dst);
    if (dst == NULL) {
        shell_builtin_write(context, "cp: cannot create destination\n");
        return -1;
    }

    offset = 0;
    while (offset < src->size) {
        size_t to_read = src->size - offset;
        if (to_read > CP_BUF_SIZE)
            to_read = CP_BUF_SIZE;

        count = fs_read(src, buffer, to_read, offset);
        if (count <= 0)
            break;

        if (fs_write(dst, buffer, (size_t)count, offset) < 0) {
            shell_builtin_write(context, "cp: write failed\n");
            return -1;
        }
        offset += (size_t)count;
    }

    return 0;
}

int shell_builtin_cp(shell_context_t *context, int argc, char *argv[]) {
    char resolved_src[SHELL_PATH_SIZE];
    char resolved_dst[SHELL_PATH_SIZE];

    if (argc < 3) {
        shell_builtin_write(context, "cp: missing operand\n");
        shell_builtin_write(context, "usage: cp <src> <dst>\n");
        return -1;
    }

    shell_resolve_path(context, argv[1], resolved_src, sizeof(resolved_src));
    shell_resolve_path(context, argv[2], resolved_dst, sizeof(resolved_dst));

    return copy_file(context, resolved_src, resolved_dst);
}
