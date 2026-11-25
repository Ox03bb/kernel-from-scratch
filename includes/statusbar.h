#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "types.h"

void statusbar_set_username(const char *username);

void statusbar_render(void);

void statusbar_pause(void);

void statusbar_resume(void);

void statusbar_run(void);

#endif
