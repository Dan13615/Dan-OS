//
// Created by dan13615 on 11/20/24.
//

#ifndef VGA_H
  #define VGA_H

    #include <stdint.h>
    #include <stdbool.h>
    #include <stddef.h>

    enum VGA_COLOR {
      PRINT_COLOR_BLACK = 0,
      PRINT_COLOR_BLUE = 1,
      PRINT_COLOR_GREEN = 2,
      PRINT_COLOR_CYAN = 3,
      PRINT_COLOR_RED = 4,
      PRINT_COLOR_MAGENTA = 5,
      PRINT_COLOR_BROWN = 6,
      PRINT_COLOR_LIGHT_GREY = 7,
      PRINT_COLOR_DARK_GREY = 8,
      PRINT_COLOR_LIGHT_BLUE = 9,
      PRINT_COLOR_LIGHT_GREEN = 10,
      PRINT_COLOR_LIGHT_CYAN = 11,
      PRINT_COLOR_LIGHT_RED = 12,
      PRINT_COLOR_PINK = 13,
      PRINT_COLOR_YELLOW = 14,
      PRINT_COLOR_WHITE = 15,
    };

    #define TTY_BACKGROUND_COLOR PRINT_COLOR_BLACK
    #define PRINT_COLOR_DEFAULT PRINT_COLOR_LIGHT_GREY
    #define PRINT_COLOR_ERROR PRINT_COLOR_RED
    #define PRINT_COLOR_PROMPT PRINT_COLOR_LIGHT_BLUE
    #define PRINT_COLOR_DANOS PRINT_COLOR_YELLOW

    #define VGA_WIDTH 80
    #define VGA_HEIGHT 25
    #define VGA_BUFFER (uint16_t*) 0xB8000

    // VGA cursor control ports
    #define VGA_CTRL_REGISTER 0x3D4
    #define VGA_DATA_REGISTER 0x3D5
    #define VGA_CURSOR_HIGH 0x0E
    #define VGA_CURSOR_LOW 0x0F

#endif //VGA_H
