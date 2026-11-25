#include "statusbar.h"

#include "driver/vga.h"
#include "sched/thread.h"
#include "timer.h"
#include "driver/rtc.h"

#include "stdio.h"

#define BAR_COLOR  ((LIGHT_GRAY << 4) | BLACK) /* black text on light gray background */

#define TIME_X     ((VGA_WIDTH - 8) / 2)        /* "hh:mm:ss"  -> 8 chars, centered */
#define DATE_X     (VGA_WIDTH - 10)             /* "dd/mm/yyyy" -> 10 chars, right aligned */
#define USER_X     0

static char statusbar_username[64] = "user";

_Noreturn static void statusbar_thread_entry(void) {
    while (1) {
        statusbar_render();
        timer_sleep_s(1);
    }
}

void statusbar_run(void) {
    thread_spawn(statusbar_thread_entry, 4096);
}

void statusbar_set_username(const char *username) {
    if (username == NULL)
        return;

    size_t i = 0;
    while (username[i] != '\0' && i < sizeof(statusbar_username) - 1) {
        statusbar_username[i] = username[i];
        i++;
    }
    statusbar_username[i] = '\0';
}

void statusbar_render(void) {
    datetime_t now;
    char time_str[9];   /* "hh:mm:ss\0"  */
    char date_str[11];  /* "dd/mm/yyyy\0" */
    uint16_t saved_cursor;

    if (rtc_get_datetime(&now) != 0) {
        /* Fall back if the RTC cannot be read. */
        const char *t = "--:--:--";
        const char *d = "--/--/----";
        size_t i;

        for (i = 0; i < 8 && t[i] != '\0'; i++)
            time_str[i] = t[i];
        time_str[i] = '\0';

        for (i = 0; i < 10 && d[i] != '\0'; i++)
            date_str[i] = d[i];
        date_str[i] = '\0';
    } else {
        snprintf(time_str, sizeof(time_str), "%d:%d:%d",
                 now.hour, now.minute, now.second);
        snprintf(date_str, sizeof(date_str), "%d/%d/%d",
                 now.day, now.month, now.year);
    }

    saved_cursor = vga_get_cursor_position();

    /* Draw the gray bar across row 0. */
    vga_clear_region(0, 0, VGA_WIDTH, 1, BAR_COLOR);

    vga_print_at(USER_X, 0, statusbar_username, BAR_COLOR);
    vga_print_at(TIME_X, 0, time_str, BAR_COLOR);
    vga_print_at(DATE_X, 0, date_str, BAR_COLOR);

    /* Restore the cursor so the shell and other output are not disturbed. */
    vga_set_cursor((uint8_t)(saved_cursor % VGA_WIDTH),
                   (uint8_t)(saved_cursor / VGA_WIDTH));
}
