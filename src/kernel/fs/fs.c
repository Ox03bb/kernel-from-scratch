#include "fs.h"
#include "heap.h"

#define FS_NAME_SIZE 64

// this is a simplified version non-persistent fs, or RamFs...

static fs_node_t *fs_root;

int fs_create_default_structure(void) {
    if (fs_mkdir("/bin") < 0)
        return -1;
    if (fs_mkdir("/dev") < 0)
        return -1;
    if (fs_mkdir("/etc") < 0)
        return -1;
    if (fs_mkdir("/home") < 0)
        return -1;
    if (fs_mkdir("/home/root") < 0)
        return -1;
    if (fs_mkdir("/proc") < 0)
        return -1;
    if (fs_mkdir("/root") < 0)
        return -1;
    if (fs_mkdir("/tmp") < 0)
        return -1;

    return 0;
}

int fs_init(void) {
    if (fs_root != NULL)
        return 0;

    fs_root = malloc(sizeof(fs_node_t));
    if (fs_root == NULL)
        return -1;

    *fs_root = (fs_node_t){
        .name = "/",
        .type = FS_DIR,
        .size = 0,
        .parent = NULL,
        .children = NULL,
        .next = NULL,
        .data = NULL,
    };

    return fs_create_default_structure();
}

static void skip_separators(const char **path) {
    while (**path == '/')
        (*path)++;
}

static int next_component(const char **path, char *component) {
    size_t length = 0;

    skip_separators(path);
    if (**path == '\0')
        return 0;

    while (**path != '\0' && **path != '/') {
        if (length >= FS_NAME_SIZE - 1)
            return -1;

        component[length++] = *(*path)++;
    }

    component[length] = '\0';
    return 1;
}

static fs_node_t *find_child(fs_node_t *directory, const char *name) {
    fs_node_t *child = directory->children;

    while (child != NULL) {
        size_t index = 0;

        while (name[index] != '\0' && child->name[index] == name[index])
            index++;

        if (name[index] == '\0' && child->name[index] == '\0')
            return child;

        child = child->next;
    }

    return NULL;
}

static fs_node_t *walk_component(fs_node_t *node, const char *component) {
    if (component[0] == '.' && component[1] == '\0')
        return node;

    if (component[0] == '.' && component[1] == '.' && component[2] == '\0') {
        return node->parent != NULL ? node->parent : node;
    }

    if (node->type != FS_DIR)
        return NULL;

    return find_child(node, component);
}

static int get_parent(const char *path, fs_node_t **parent, char *name) {
    const char *cursor = path;
    fs_node_t *current = fs_root;
    char component[FS_NAME_SIZE];
    int result;

    if (path == NULL || path[0] == '\0' || fs_root == NULL)
        return -1;

    while (1) {
        result = next_component(&cursor, component);
        if (result <= 0)
            return -1;

        skip_separators(&cursor);
        if (*cursor == '\0') {
            if ((component[0] == '.' && component[1] == '\0') ||
                (component[0] == '.' && component[1] == '.' && component[2] == '\0'))
                return -1;

            size_t index = 0;
            while (component[index] != '\0') {
                name[index] = component[index];
                index++;
            }
            name[index] = '\0';
            *parent = current;
            return 0;
        }

        current = walk_component(current, component);
        if (current == NULL || current->type != FS_DIR)
            return -1;
    }
}

static int create_node(const char *path, enum fs_type type) {
    fs_node_t *parent;
    fs_node_t *node;
    char name[FS_NAME_SIZE];
    size_t index;

    if (get_parent(path, &parent, name) < 0 || parent->type != FS_DIR)
        return -1;

    if (find_child(parent, name) != NULL)
        return -1;

    node = malloc(sizeof(fs_node_t));
    if (node == NULL)
        return -1;

    *node = (fs_node_t){
        .type = type,
        .parent = parent,
    };

    for (index = 0; name[index] != '\0'; index++)
        node->name[index] = name[index];
    node->name[index] = '\0';

    node->next = parent->children;
    parent->children = node;
    return 0;
}

int fs_mkdir(const char *path) { return create_node(path, FS_DIR); }

int fs_create(const char *path) { return create_node(path, FS_FILE); }

int fs_unlink(const char *path) {
    fs_node_t *node;
    fs_node_t *parent;
    fs_node_t *current;

    node = fs_lookup(path);
    if (node == NULL || node == fs_root || node->children != NULL)
        return -1;

    parent = node->parent;
    current = parent->children;
    if (current == node) {
        parent->children = node->next;
    } else {
        while (current != NULL && current->next != node)
            current = current->next;

        if (current == NULL)
            return -1;
        current->next = node->next;
    }

    if (node->data != NULL && free(node->data) < 0)
        return -1;
    return free(node);
}

fs_node_t *fs_lookup(const char *path) {
    const char *cursor = path;
    fs_node_t *current = fs_root;
    char component[FS_NAME_SIZE];
    int result;

    if (path == NULL || fs_root == NULL || path[0] == '\0')
        return NULL;

    while ((result = next_component(&cursor, component)) > 0) {
        current = walk_component(current, component);
        if (current == NULL)
            return NULL;
    }

    return result < 0 ? NULL : current;
}

int fs_read(fs_node_t *node, void *buffer, size_t size, size_t offset) {
    size_t available;
    size_t index;
    unsigned char *destination = buffer;
    const unsigned char *source;

    if (node == NULL || node->type != FS_FILE || buffer == NULL)
        return -1;
    if (offset >= node->size || size == 0)
        return 0;

    available = node->size - offset;
    if (size > available)
        size = available;

    source = (const unsigned char *)node->data + offset;
    for (index = 0; index < size; index++)
        destination[index] = source[index];

    return (int)size;
}

int fs_write(fs_node_t *node, const void *buffer, size_t size, size_t offset) {
    size_t end;
    size_t index;
    unsigned char *data;
    const unsigned char *source = buffer;

    if (node == NULL || node->type != FS_FILE || (buffer == NULL && size != 0))
        return -1;
    if (size == 0)
        return 0;
    if (offset > (size_t)-1 - size)
        return -1;

    end = offset + size;
    if (end > node->size) {
        data = malloc(end);
        if (data == NULL)
            return -1;

        for (index = 0; index < node->size; index++)
            data[index] = ((unsigned char *)node->data)[index];
        for (; index < end; index++)
            data[index] = 0;

        if (node->data != NULL)
            free(node->data);
        node->data = data;
        node->size = end;
    }

    data = (unsigned char *)node->data;
    for (index = 0; index < size; index++)
        data[offset + index] = source[index];

    return (int)size;
}

int fs_readdir(fs_node_t *node) {
    int count = 0;
    fs_node_t *child;

    if (node == NULL || node->type != FS_DIR)
        return -1;

    child = node->children;
    while (child != NULL) {
        count++;
        child = child->next;
    }

    return count;
}
