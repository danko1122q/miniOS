#ifndef STRING_H
#define STRING_H

#include <stdint.h>

int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int n);
char *strcat(char *dest, const char *src);
int memcmp(const void *s1, const void *s2, int n);
void *memcpy(void *dest, const void *src, int n);
void *memset(void *s, int c, int n);

#endif
