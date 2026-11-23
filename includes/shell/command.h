#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include "shell/shell.h"

typedef int (*shell_command_fn)(shell_context_t *context, int argc, char *argv[]);

typedef struct shell_command {
    const char *name;
    const char *description;
    shell_command_fn handler;
    struct shell_command *next;
} shell_command_t;

void shell_command_registry_init(void);
int shell_command_register(const char *name, const char *description, shell_command_fn handler);
int shell_command_execute(shell_context_t *context, int argc, char *argv[]);

#endif
