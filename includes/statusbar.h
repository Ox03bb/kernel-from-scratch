#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "types.h"

/* Set the user name shown on the left side of the status bar (from the tty). */
void statusbar_set_username(const char *username);

/* Draw the gray status bar on row 0: user | hh:mm:ss | dd/mm/yyyy.
 * Saves and restores the VGA cursor so other screen output is undisturbed. */
void statusbar_render(void);

/* Spawn the status bar as its own thread, updating once per second. */
void statusbar_run(void);

#endif
