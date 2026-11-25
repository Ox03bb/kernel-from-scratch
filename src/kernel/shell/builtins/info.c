#include "shell/builtins/builtins.h"

#include "stdio.h"
#include "timer.h"

int shell_builtin_info(shell_context_t *context, int argc, char *argv[]) {
    // hard coded information for now, can be improved later

    (void)context;
    (void)argc;
    (void)argv;

    printf("kernel-from-scratch\n");
    printf("  uptime: %d seconds\n", timer_get_seconds());
    printf("  architecture: x86\n");
    printf("  kernel version: 0.1.0\n");
    printf("  C standard: C99\n");
    printf("  Memory: 4 GB\n");

    return 0;
}
