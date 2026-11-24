#ifndef SHELL_BUILTINS_H
#define SHELL_BUILTINS_H

#include "shell/command.h"

void shell_builtin_write(shell_context_t *context, const char *text);

int shell_builtin_help(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_clear(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_echo(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_pwd(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_ls(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_cd(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_mkdir(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_touch(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_rm(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_cat(shell_context_t *context, int argc, char *argv[]);
int shell_builtin_tree(shell_context_t *context, int argc, char *argv[]);

void shell_register_builtins(void);

#endif
