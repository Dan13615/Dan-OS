/**========================================================================
 *                             Compare utils
 *  description: This file contains utility functions for comparing strings.
 *  author: Daniil Stepanov
 *  date: 2025-10-03
 *========================================================================**/

#include "string.h"

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n-- > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    if (n == (size_t)-1) {
        return 0; // Equal up to n characters
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}