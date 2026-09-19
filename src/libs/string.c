#include "types.h"
#include "string.h"

int strlen(char *str){
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}
void strcpy(char *dest, char *src){
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}
void strcat(char *dest, char *src){
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

char *strchr(const char *str, int c){
    while (*str) {
        if (*str == (char)c) {
            return (char *)str;
        }
        str++;
    }
    return NULL;
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