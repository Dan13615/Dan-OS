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