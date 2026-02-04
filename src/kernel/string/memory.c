/**========================================================================
 *                             Memory Functions
 *  description: This module provides basic memory manipulation functions.
 *  author: Daniil Stepanov
 *  date: 2025-10-02
 *========================================================================**/

#include "string.h"

void memset(void* dest, int value, size_t len) {
    unsigned char* ptr = (unsigned char*)dest;
    for (size_t i = 0; i < len; i++) {
        ptr[i] = (unsigned char)value;
    }
}

void memcpy(void* dest, const void* src, size_t len) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < len; i++) {
        d[i] = s[i];
    }
}

int memcmp(void* dest, const void* src, size_t len) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for (size_t i = 0; i < len; i++) {
        if (d[i] != s[i]) {
            return (d[i] - s[i]);
        }
    }
    return 0;
}