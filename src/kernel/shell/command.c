#include "shell/command.h"
#include "heap.h"

static shell_command_t *command_registry;

void shell_command_registry_init(void) { command_registry = NULL; }

int shell_command_register(const char *name, const char *description, shell_command_fn handler) {
    shell_command_t *command;

    if (name == NULL || name[0] == '\0' || handler == NULL)
        return -1;

    command = malloc(sizeof(shell_command_t));
    if (command == NULL)
        return -1;

    command->name = name;
    command->description = description;
    command->handler = handler;
    command->next = command_registry;
    command_registry = command;
    return 0;
}

int shell_command_execute(shell_context_t *context, int argc, char *argv[]) {
    shell_command_t *command;

    if (context == NULL || argc == 0)
        return 0;

    for (command = command_registry; command != NULL; command = command->next) {
        const char *left = command->name;
        const char *right = argv[0];

        while (*left != '\0' && *right != '\0' && *left == *right) {
            left++;
            right++;
        }

        if (*left == '\0' && *right == '\0')
            return command->handler(context, argc, argv);
    }

    tty_t *tty = context->tty;
    if (tty != NULL) {
        tty->method.write(tty, "Unknown command: ");
        tty->method.write(tty, argv[0]);
        tty->method.write(tty, "\n");
    }
    return -1;
}
