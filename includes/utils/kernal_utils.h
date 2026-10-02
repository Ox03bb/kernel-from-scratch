#ifndef KERNAL_UTILS_H
#define KERNAL_UTILS_H

void KERNEL_INIT(const char *name, void (*init_function)(void));
void KERNEL_INIT_P(const char *name);
void timer_setup(void);

#endif
