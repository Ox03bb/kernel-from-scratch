#ifndef STRING_H
#define STRING_H

#include "types.h"

int strlen(char *str);

void strcpy(char *dest, char *src);
void strcat(char *dest, char *src);

int strcmp(const char *str1, const char *str2);

char *strchr(const char *str, char c); //find first occurrence of character c in string str

char *strstr(const char *str, const char *substr); // find first occurrence of substring substr in string str

#endif

