#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include <stdarg.h>

void print(const char *str);
void println(const char *str);
void print_at_end(const char *str, uint8_t color);

void printf(const char *format, ...);
void snprintf(char *buffer, size_t buffer_size, const char *format, ...);

char *vformatter(const char *format, va_list args);
char *formatter(const char *format, ...);

#endif