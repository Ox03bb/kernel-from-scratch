#ifndef FS_H
#define FS_H

#include "types.h"

enum fs_type { FS_DIR, FS_FILE };

typedef struct fs_node {
    char name[64];
    enum fs_type type;
    size_t size;

    struct fs_node *parent;
    struct fs_node *children;
    struct fs_node *next;

    void *data;
} fs_node_t;

int fs_init(void);

int fs_mkdir(const char *path);
int fs_create(const char *path);
int fs_unlink(const char *path);

fs_node_t *fs_lookup(const char *path);

int fs_read(fs_node_t *node, void *buffer, size_t size, size_t offset);

int fs_write(fs_node_t *node, const void *buffer, size_t size, size_t offset);

int fs_readdir(fs_node_t *node);

#endif