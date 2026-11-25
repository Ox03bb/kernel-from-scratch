#include "shell/builtins/builtins.h"

#include "timer.h"

static unsigned long parse_ulong(const char *s) {
    unsigned long value = 0;

    while (*s >= '0' && *s <= '9') {
        value = value * 10 + (unsigned long)(*s - '0');
        s++;
    }

    return value;
}

int shell_builtin_sleep(shell_context_t *context, int argc, char *argv[]) {
    if (argc < 2) {
        shell_builtin_write(context, "sleep: missing operand\n");
        shell_builtin_write(context, "usage: sleep <seconds>\n");
        return -1;
    }

    timer_sleep_s((uint32_t)parse_ulong(argv[1]));
    return 0;
}
