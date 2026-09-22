#include <stdarg.h>

#include "types.h"
#include "vga.h"
#include "vga_lib.h"

#include "string.h"

#include "stdio.h"

#define FORMATTER_BUFFER_SIZE 1024
static char buffer[FORMATTER_BUFFER_SIZE];

void print(const char *str) {
    while (*str) {

        if (*str == '\033') {
            str++;

            if (*str == '[') {
                str++;

                if (*str == '3') {
                    str++;

                    switch (*str) {
                    case '0':
                        vga_set_color(BLACK, DEFUALT_B);
                        break;

                    case '1':
                        vga_set_color(RED, DEFUALT_B);
                        break;

                    case '2':
                        vga_set_color(GREEN, DEFUALT_B);
                        break;

                    case '3':
                        vga_set_color(YELLOW, DEFUALT_B);
                        break;

                    case '4':
                        vga_set_color(BLUE, DEFUALT_B);
                        break;

                    case '5':
                        vga_set_color(MAGENTA, DEFUALT_B);
                        break;

                    case '6':
                        vga_set_color(CYAN, DEFUALT_B);
                        break;

                    case '7':
                        vga_set_color(WHITE, DEFUALT_B);
                        break;
                    }

                    if (*str)
                        str++;

                    if (*str == 'm')
                        str++;
                } else if (*str == '0') {
                    str++;

                    if (*str == 'm') {
                        vga_set_color(DEFUALT_F, DEFUALT_B);
                        str++;
                    }
                }
            }

            continue;
        }

        vga_print_char(*str);
        str++;
    }
}

void println(const char *str) {
    vga_print("\n");
    vga_print(str);
}

void print_char(char c) { vga_print_char(c); }

void print_hex(uint32_t value) { vga_print_hex(value); }

void print_at_end(const char *str, uint8_t color) { vga_print_at_end_c(str, color); }

void printf(const char *format, ...) {
    va_list args;

    va_start(args, format);

    char *result = vformatter(format, args);

    va_end(args);

    print(result);
}

void snprintf(char *buffer, size_t buffer_size, const char *format, ...) {
    va_list args;

    va_start(args, format);

    char *result = vformatter(format, args);

    va_end(args);

    size_t i;
    for (i = 0; i < buffer_size - 1 && result[i] != '\0'; i++) {
        buffer[i] = result[i];
    }
    buffer[i] = '\0';
}

char *formatter(const char *format, ...) {
    va_list args;

    va_start(args, format);

    char *result = vformatter(format, args);

    va_end(args);

    return result;
}

char *vformatter(const char *format, va_list args) {
    buffer[0] = '\0';
    size_t used = 0;

    while (*format) {
        if (*format != '%') {
            char temp[2] = {*format, '\0'};
            size_t len = strlen(temp);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, temp);
                used += len;
            }
            format++;
            continue;
        }

        format++;
        if (*format == '\0') {
            if (used + 1 < sizeof(buffer)) {
                buffer[used++] = '%';
                buffer[used] = '\0';
            }
            break;
        }

        switch (*format) {

        case 's': {
            const char *str = va_arg(args, const char *);
            size_t len = strlen(str);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, str);
                used += len;
            }
            break;
        }

        case 'd': {
            int number = va_arg(args, int);
            char *str = int2str(number);
            size_t len = strlen(str);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, str);
                used += len;
            }
            break;
        }

        case 'x': {
            uint32_t number = va_arg(args, uint32_t);
            char *str = uint2str(number, 16, false);
            size_t len = strlen(str);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, str);
                used += len;
            }
            break;
        }

        case 'h': {
            uint32_t number = va_arg(args, uint32_t);
            char *str = uint2str(number, 16, true);
            size_t len = strlen(str);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, str);
                used += len;
            }
            break;
        }

        case 'b': {
            uint32_t number = va_arg(args, uint32_t);
            char *str = uint2str(number, 2, false);
            size_t len = strlen(str);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, str);
                used += len;
            }
            break;
        }

        case 'c': {
            char temp[2];
            temp[0] = (char)va_arg(args, int);
            temp[1] = '\0';
            size_t len = strlen(temp);
            if (used + len + 1 < sizeof(buffer)) {
                strcat(buffer, temp);
                used += len;
            }
            break;
        }

        case '%': {
            if (used + 2 < sizeof(buffer)) {
                strcat(buffer, "%");
                used++;
            }
            break;
        }

        default: {
            if (used + 2 < sizeof(buffer)) {
                strcat(buffer, "%");
                used++;
            }
            break;
        }
        }

        format++;
    }

    return buffer;
}
