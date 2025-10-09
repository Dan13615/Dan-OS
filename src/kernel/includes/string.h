//
// Created by dan13615 on 11/20/24.
//

#ifndef STR_H
  #define STR_H

  #include <stddef.h>

  int strlen(const char* str);
  void memset(void* dest, int value, size_t len);
  int strcmp(const char* s1, const char* s2);
  int strncmp(const char* s1, const char* s2, size_t n);
#endif //STR_H
