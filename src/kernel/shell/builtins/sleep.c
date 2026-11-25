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

/*
 * Sleep for at most `total_ms` milliseconds, but poll for Ctrl+C every
 * `SLEEP_POLL_MS` so the command can be interrupted. Returns 0 when the
 * full time elapsed, -1 when interrupted.
 */
static int sleep_interruptible(shell_context_t *context, uint32_t total_ms) {
    tty_t *tty = context->tty;
    uint32_t elapsed = 0;
    uint32_t chunk_ms = 100;

    while (elapsed < total_ms) {
        /* Drain pending keyboard events so a Ctrl+C is noticed. */
        tty_process_events(tty);

        if (tty->interrupt_requested) {
            tty->interrupt_requested = false;
            shell_builtin_write(context, "\nsleep: interrupted\n");
            return -1;
        }

        /*
         * Busy-wait one poll chunk. timer_get_frequency() is the PIT tick
         * rate (ticks per second).
         */
        uint32_t start = timer_get_ticks();
        uint32_t wait_ticks = (chunk_ms * timer_get_frequency()) / 1000;
        while (timer_get_ticks() - start < wait_ticks) {
            /* spin */
        }

        elapsed += chunk_ms;
    }

    return 0;
}

int shell_builtin_sleep(shell_context_t *context, int argc, char *argv[]) {
    if (argc < 2) {
        shell_builtin_write(context, "sleep: missing operand\n");
        shell_builtin_write(context, "usage: sleep <seconds>\n");
        return -1;
    }

    uint32_t seconds = (uint32_t)parse_ulong(argv[1]);
    sleep_interruptible(context, seconds * 1000);

    return 0;
}
