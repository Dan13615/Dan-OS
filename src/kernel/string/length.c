//
// Created by dan13615 on 11/20/24.
//

#include "string.h"

int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0')
      len++;
    return len;
}