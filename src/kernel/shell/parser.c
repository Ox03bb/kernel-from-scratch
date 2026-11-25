#include "shell/parser.h"

int shell_parse(char *line, char *argv[SHELL_ARG_MAX]) {
    int argc = 0;
    char *cursor = line;

    if (line == NULL)
        return 0;

    while (*cursor != '\0') {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n')
            cursor++;

        if (*cursor == '\0')
            break;

        if (argc >= SHELL_ARG_MAX)
            return argc;

        argv[argc++] = cursor;

        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '\r' &&
               *cursor != '\n')
            cursor++;

        if (*cursor != '\0')
            *cursor++ = '\0';
    }

    argv[argc] = NULL;
    return argc;
}
