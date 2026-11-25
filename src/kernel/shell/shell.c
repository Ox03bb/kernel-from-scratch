#include "shell/shell.h"

#include "shell/builtins/builtins.h"
#include "shell/command.h"
#include "shell/parser.h"
#include "stdio.h"
#include "string.h"

static void copy_string(char *destination, const char *source, size_t size) {
    size_t i = 0;

    if (size == 0)
        return;

    while (i < size - 1 && source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

static bool strings_equal(const char *left, const char *right) {
    if (left == NULL || right == NULL)
        return false;

    while (*left != '\0' && *right != '\0') {
        if (*left != *right)
            return false;
        left++;
        right++;
    }
    return *left == '\0' && *right == '\0';
}

static void trim_line_end(char *line) {
    size_t length;

    if (line == NULL)
        return;

    length = strlen(line);
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
        line[--length] = '\0';
}

void shell_history_add(shell_context_t *context, const char *line) {
    char entry[SHELL_LINE_SIZE];
    size_t index;

    if (context == NULL || line == NULL || line[0] == '\0')
        return;

    copy_string(entry, line, sizeof(entry));
    trim_line_end(entry);
    if (entry[0] == '\0')
        return;

    if (context->history_count < SHELL_HISTORY_MAX) {
        index = context->history_count++;
    } else {
        for (index = 1; index < SHELL_HISTORY_MAX; index++)
            copy_string(context->history[index - 1], context->history[index],
                        sizeof(context->history[index]));
        index = SHELL_HISTORY_MAX - 1;
    }

    copy_string(context->history[index], entry, sizeof(context->history[index]));
}

static int shell_history_count_cb(void *context) {
    shell_context_t *ctx = (shell_context_t *)context;

    if (ctx == NULL)
        return 0;

    return (int)ctx->history_count;
}

static const char *shell_history_at_cb(void *context, int index) {
    shell_context_t *ctx = (shell_context_t *)context;

    if (ctx == NULL || index < 0 || (size_t)index >= ctx->history_count)
        return NULL;

    return ctx->history[index];
}

const char *shell_env_get(const shell_context_t *context, const char *key) {
    if (context == NULL || key == NULL)
        return NULL;

    for (size_t i = 0; i < SHELL_ENV_MAX; i++) {
        if (context->env[i].used && strings_equal(context->env[i].key, key))
            return context->env[i].value;
    }
    return NULL;
}

int shell_env_set(shell_context_t *context, const char *key, const char *value) {
    if (context == NULL || key == NULL || key[0] == '\0' || value == NULL)
        return -1;

    for (size_t i = 0; i < SHELL_ENV_MAX; i++) {
        if (context->env[i].used && strings_equal(context->env[i].key, key)) {
            copy_string(context->env[i].value, value, sizeof(context->env[i].value));
            return 0;
        }
    }

    for (size_t i = 0; i < SHELL_ENV_MAX; i++) {
        if (!context->env[i].used) {
            context->env[i].used = true;
            copy_string(context->env[i].key, key, sizeof(context->env[i].key));
            copy_string(context->env[i].value, value, sizeof(context->env[i].value));
            return 0;
        }
    }
    return -1;
}

void shell_resolve_path(const shell_context_t *context, const char *path, char *resolved,
                        size_t resolved_size) {
    char candidate[SHELL_PATH_SIZE * 2];
    char normalized[SHELL_PATH_SIZE];
    size_t candidate_length = 0;
    size_t i = 0;

    if (resolved_size == 0)
        return;

    if (path != NULL && path[0] == '/') {
        copy_string(candidate, path, sizeof(candidate));
    } else {
        candidate[0] = '\0';
        copy_string(candidate, context->currentDir, sizeof(candidate));
        candidate_length = strlen(candidate);

        if (path != NULL && path[0] != '\0') {
            if (candidate_length > 0 && candidate[candidate_length - 1] != '/')
                candidate[candidate_length++] = '/';
            copy_string(candidate + candidate_length, path, sizeof(candidate) - candidate_length);
        }
    }

    normalized[0] = '/';
    normalized[1] = '\0';

    while (candidate[i] != '\0') {
        char component[SHELL_PATH_SIZE];
        size_t component_length = 0;

        while (candidate[i] == '/')
            i++;
        if (candidate[i] == '\0')
            break;

        while (candidate[i] != '\0' && candidate[i] != '/') {
            if (component_length < sizeof(component) - 1)
                component[component_length++] = candidate[i];
            i++;
        }
        component[component_length] = '\0';

        if (component_length == 1 && component[0] == '.')
            continue;

        if (component_length == 2 && component[0] == '.' && component[1] == '.') {
            size_t length = strlen(normalized);
            if (length > 1) {
                while (length > 1 && normalized[length - 1] != '/')
                    length--;
                if (normalized[length - 1] == '/' && length > 1)
                    length--;
                normalized[length] = '\0';

                if (normalized[0] == '\0') {
                    normalized[0] = '/';
                    normalized[1] = '\0';
                }
            }
            continue;
        }

        size_t length = strlen(normalized);
        if (length > 1) {
            normalized[length] = '/';
            normalized[length + 1] = '\0';
        }

        length = strlen(normalized);
        if (length + component_length < sizeof(normalized)) {
            for (size_t j = 0; j < component_length; j++)
                normalized[length + j] = component[j];
            normalized[length + component_length] = '\0';
        }
    }

    copy_string(resolved, normalized, resolved_size);
}

int shell_init(shell_context_t *context, tty_t *tty) {
    if (context == NULL || tty == NULL)
        return -1;

    for (size_t i = 0; i < sizeof(*context); i++)
        ((volatile char *)context)[i] = 0;

    context->tty = tty;
    tty_set_history_callbacks(context->tty, context,
                              shell_history_count_cb, shell_history_at_cb);
    copy_string(context->name, "Ali", sizeof(context->name));
    copy_string(context->user, "root", sizeof(context->user));
    context->currentDir[0] = '/';
    context->currentDir[1] = '\0';
    context->config.echo = true;
    context->config.show_prompt = true;
    tty->echo = context->config.echo;
    copy_string(context->config.prompt, "$ ", sizeof(context->config.prompt));

    shell_env_set(context, "USER", "root");
    shell_env_set(context, "HOME", "/root");
    shell_env_set(context, "PATH", "/bin:/sbin");

    shell_command_registry_init();
    shell_register_builtins();
    shell_prompt(context);
    return 0;
}

void shell_prompt(shell_context_t *context) {
    if (context == NULL || context->tty == NULL)
        return;
    if (!context->config.show_prompt)
        return;

    printf("\n\033[0m[ \033[02:00m%s\033[0m ]\n"
           "\033[11:00m%s\033[14:00m@kernel-fs\033[0m %s",
           context->currentDir,
           context->name,
           context->config.prompt);
}

void shell_process(shell_context_t *context) {
    char line[SHELL_LINE_SIZE];
    char *argv[SHELL_ARG_MAX + 1];

    if (context == NULL || context->tty == NULL)
        return;

    tty_process_events(context->tty);
    if (!context->tty->line_ready)
        return;

    context->tty->method.read(context->tty, line, sizeof(line));
    trim_line_end(line);
    copy_string(context->rawInput, line, sizeof(context->rawInput));
    shell_history_add(context, line);

    context->tty->line_ready = false;
    context->tty->buffer_index = 0;
    context->tty->cursor_index = 0;
    context->tty->input_length = 0;
    context->tty->buffer[0] = '\0';

    int argc = shell_parse(line, argv);
    if (argc > 0)
        shell_command_execute(context, argc, argv);

    shell_prompt(context);
}
