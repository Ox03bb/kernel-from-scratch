#include "shell/builtins/builtins.h"

void shell_builtin_write(shell_context_t *context, const char *text) {
    if (context != NULL && context->tty != NULL)
        context->tty->method.write(context->tty, text);
}

void shell_register_builtins(void) {
    shell_command_register("help", "Show built-in commands", shell_builtin_help);
    shell_command_register("clear", "Clear the screen", shell_builtin_clear);
    shell_command_register("echo", "Print text", shell_builtin_echo);
    shell_command_register("pwd", "Print working directory", shell_builtin_pwd);
    shell_command_register("ls", "List directory contents", shell_builtin_ls);
    shell_command_register("cd", "Change directory", shell_builtin_cd);
    shell_command_register("mkdir", "Create a directory", shell_builtin_mkdir);
    shell_command_register("touch", "Create a file", shell_builtin_touch);
    shell_command_register("rm", "Remove a file or empty directory", shell_builtin_rm);
    shell_command_register("cat", "Print a file", shell_builtin_cat);
    shell_command_register("tree", "Recursively list directory contents", shell_builtin_tree);
    shell_command_register("ed", "Edit a file", shell_builtin_ed);
}
