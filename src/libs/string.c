#include <stddef.h>
#include "types.h"
#include "string.h"

int strlen(char *str){
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

char *int2str(int num)
{
    static char str[33];

    char *p = str + 32;

    *p = '\0';

    if (num == 0)
    {
        *(--p) = '0';
    }
    else
    {
        bool negative = num < 0;

        if (negative)
            num = -num;

        while (num > 0)
        {
            *(--p) = (num % 10) + '0';
            num /= 10;
        }

        if (negative)
            *(--p) = '-';
    }

    return p;
}

void strcpy(char *dest, char *src){
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void strcat(char *dest,const char *src){
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}


int strcmp(const char *str1, const char *str2){
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

char *strchr(const char *str, int c)
{
    while (*str != '\0') {
        if (*str == (char)c) {
            return (char *)str;
        }

        str++;
    }

    return (c == '\0') ? (char *)str : NULL;
}


char *strstr(const char *str, const char *substr){
    if (!*substr) {
        return (char *)str;
    }
    for (; *str; str++) {
        if (*str == *substr) {
            const char *h, *n;
            for (h = str, n = substr; *h && *n && *h == *n; ++h, ++n);
            if (!*n) {
                return (char *)str;
            }
        }
    }
    return NULL;
}