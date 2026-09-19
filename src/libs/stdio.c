#include <stdarg.h>

#include "types.h"
#include "vga.h"
#include "vga_lib.h"

#include "string.h"

#include "stdio.h"

#define FORMATTER_BUFFER_SIZE 1024
static char buffer[FORMATTER_BUFFER_SIZE];


void print(const char *str)
{
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
                }else if (*str == '0') {
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

void println(const char *str){
    vga_print("\n");
    vga_print(str);
}

void print_at_end(const char *str, uint8_t color) {
    vga_print_at_end_c(str, color);
}

void printf(const char *format, ...){
    va_list args;

    va_start(args, format);

    char *result = vformatter(format, args);

    va_end(args);
    
    vga_print(result);
}

void snprintf(char *buffer, size_t buffer_size, const char *format, ...){
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


char *formatter(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    char *result = vformatter(format, args);

    va_end(args);

    return result;
}

char *vformatter(const char *format, va_list args)
{
    buffer[0] = '\0';

    while (*format)
    {
        if (*format != '%')
        {
            char temp[2] = {*format, '\0'};
            strcat(buffer, temp);

            format++;
            continue;
        }

        format++;

        switch (*format)
        {
            case 's':
            {
                const char *str = va_arg(args, const char *);
                strcat(buffer, str);
                break;
            }

            case 'd':
            {
                int number = va_arg(args, int);
                strcat(buffer, int2str(number));
                break;
            }

            case 'c':
            {
                char temp[2];

                temp[0] = (char)va_arg(args, int);
                temp[1] = '\0';

                strcat(buffer, temp);
                break;
            }

            case '%':
            {
                strcat(buffer, "%");
                break;
            }

            default:
            {
                strcat(buffer, "%");
                break;
            }
        }

        format++;
    }

    return buffer;
}