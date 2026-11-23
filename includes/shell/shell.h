#ifndef SHELL_H
#define SHELL_H

#include "driver/tty.h"

#define SHELL_LINE_SIZE      256
#define SHELL_PATH_SIZE      128
#define SHELL_ARG_MAX        32
#define SHELL_NAME_SIZE      64
#define SHELL_USER_SIZE      64
#define SHELL_HISTORY_MAX    16
#define SHELL_ENV_MAX        32
#define SHELL_ENV_KEY_SIZE   64
#define SHELL_ENV_VALUE_SIZE 128

//! NOTE: this shell is AI-generated implementation based on my previous project, gsh
//! (https://github.com/Ox03bb/gsh), a shell I built from scratch in Go, ported to C with some
//! modifications for this kernel.

typedef struct {
    char key[SHELL_ENV_KEY_SIZE];
    char value[SHELL_ENV_VALUE_SIZE];
    bool used;
} shell_env_entry_t;

typedef struct {
    bool echo;
    bool show_prompt;
    char prompt[SHELL_NAME_SIZE];
} shell_config_t;

typedef struct {
    tty_t *tty;
    char name[SHELL_NAME_SIZE];
    char user[SHELL_USER_SIZE];
    char currentDir[SHELL_PATH_SIZE];
    char rawInput[SHELL_LINE_SIZE];
    char history[SHELL_HISTORY_MAX][SHELL_LINE_SIZE];
    size_t history_count;
    shell_env_entry_t env[SHELL_ENV_MAX];
    shell_config_t config;
} shell_context_t;

int shell_init(shell_context_t *context, tty_t *tty);
void shell_process(shell_context_t *context);
void shell_prompt(shell_context_t *context);

void shell_resolve_path(const shell_context_t *context, const char *path, char *resolved,
                        size_t resolved_size);
void shell_history_add(shell_context_t *context, const char *line);
const char *shell_env_get(const shell_context_t *context, const char *key);
int shell_env_set(shell_context_t *context, const char *key, const char *value);

#endif
