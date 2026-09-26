#include <stdarg.h>

#include "types.h"
#include "vga.h"
#include "vga_lib.h"

#include "string.h"
#include "keyboard/event_queue.h"

#include "stdio.h"

#define FORMATTER_BUFFER_SIZE 1024
static char buffer[FORMATTER_BUFFER_SIZE];
static bool scan_has_pending;
static char scan_pending;

static char scan_read_char(void) {
    input_event_t event;

    if (scan_has_pending) {
        scan_has_pending = false;
        return scan_pending;
    }

    while (input_queue_pop(&event)) {
        if (event.action != KEY_PRESSED)
            continue;

        if (event.type == INPUT_EVENT_CHAR)
            return event.character;

        if (event.key == KEY_ENTER)
            return '\n';
    }

    return '\0';
}

static void scan_unread_char(char character) {
    scan_pending = character;
    scan_has_pending = true;
}

static bool scan_is_space(char character) {
    return character == ' ' || character == '\t' || character == '\n' || character == '\r';
}

char scan_char(void) {
    char character;

    do {
        character = scan_read_char();
    } while (character == '\0');

    return character;
}

int scanf(const char *format, ...) {
    va_list args;
    int assigned = 0;

    va_start(args, format);

    while (*format) {
        if (scan_is_space(*format)) {
            while (scan_is_space(*format))
                format++;

            char character;
            do {
                character = scan_char();
            } while (scan_is_space(character));
            scan_unread_char(character);
            continue;
        }

        if (*format != '%') {
            char character = scan_char();
            if (character != *format) {
                scan_unread_char(character);
                break;
            }
            format++;
            continue;
        }

        format++;
        if (*format == '\0')
            break;

        switch (*format) {
        case 'c': {
            char *value = va_arg(args, char *);
            *value = scan_char();
            assigned++;
            break;
        }

        case 's': {
            char *value = va_arg(args, char *);
            char character;
            size_t length = 0;

            do {
                character = scan_char();
            } while (scan_is_space(character));

            while (!scan_is_space(character)) {
                value[length++] = character;
                character = scan_char();
            }
            value[length] = '\0';
            assigned++;
            break;
        }

        case 'd': {
            int *value = va_arg(args, int *);
            char character;
            int sign = 1;
            int number = 0;

            do {
                character = scan_char();
            } while (scan_is_space(character));

            if (character == '-' || character == '+') {
                sign = character == '-' ? -1 : 1;
                character = scan_char();
            }

            if (character < '0' || character > '9') {
                scan_unread_char(character);
                break;
            }

            do {
                number = number * 10 + (character - '0');
                character = scan_char();
            } while (character >= '0' && character <= '9');

            scan_unread_char(character);
            *value = number * sign;
            assigned++;
            break;
        }

        case 'x': {
            uint32_t *value = va_arg(args, uint32_t *);
            char character;
            uint32_t number = 0;
            bool has_digit = false;

            do {
                character = scan_char();
            } while (scan_is_space(character));

            if (character == '0') {
                char prefix = scan_char();
                if (prefix == 'x' || prefix == 'X')
                    character = scan_char();
                else
                    scan_unread_char(prefix);
            }

            while ((character >= '0' && character <= '9') ||
                   (character >= 'a' && character <= 'f') ||
                   (character >= 'A' && character <= 'F')) {
                uint32_t digit;
                if (character >= '0' && character <= '9')
                    digit = (uint32_t)(character - '0');
                else if (character >= 'a' && character <= 'f')
                    digit = (uint32_t)(character - 'a' + 10);
                else
                    digit = (uint32_t)(character - 'A' + 10);

                number = number * 16 + digit;
                has_digit = true;
                character = scan_char();
            }

            if (!has_digit) {
                scan_unread_char(character);
                break;
            }

            scan_unread_char(character);
            *value = number;
            assigned++;
            break;
        }

        default:
            scan_unread_char('%');
            va_end(args);
            return assigned;
        }

        format++;
    }

    va_end(args);
    return assigned;
}

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
