//
// Created by dan13615 on 11/15/24.
//

#include <stddef.h>
#include "string.h"
#include "tty.h"
#include "vga.h"

static size_t tty_row;
static size_t tty_column;
static uint8_t tty_color;
static uint16_t* tty_buffer;
static uint16_t* const VGA_MEMORY = VGA_BUFFER;

extern void ioport_out(unsigned short port, unsigned char value);
extern unsigned char ioport_in(unsigned short port);

static inline uint8_t vga_entry_color(enum VGA_COLOR fg, enum VGA_COLOR bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static void set_hardware_cursor(size_t x, size_t y) {
    uint16_t position = y * VGA_WIDTH + x;
    ioport_out(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    ioport_out(VGA_DATA_REGISTER, (position >> 8) & 0xFF);
    ioport_out(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    ioport_out(VGA_DATA_REGISTER, position & 0xFF);
}

static uint16_t get_hardware_cursor(void) {
    uint16_t position = 0;

    ioport_out(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    position |= ((uint16_t)ioport_in(VGA_DATA_REGISTER)) << 8;
    ioport_out(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    position |= ioport_in(VGA_DATA_REGISTER);
    return position;
}

static void set_cursor_visibility(int visible) {
    ioport_out(VGA_CTRL_REGISTER, 0x0A);
    if (visible) {
        ioport_out(VGA_DATA_REGISTER, 0x0E);
    } else {
        ioport_out(VGA_DATA_REGISTER, 0x20);
    }
    ioport_out(VGA_CTRL_REGISTER, 0x0B);
    ioport_out(VGA_DATA_REGISTER, 0x0F);
}

void tty_cursor_backspace(size_t count) {
    while (count-- > 0) {
        if (tty_column == 0) {
            if (tty_row > 0) {
                tty_row--;
                tty_column = VGA_WIDTH - 1;
            }
        } else {
            tty_column--;
        }
        tty_putchar_at(' ', tty_color, tty_column, tty_row);
    }
    set_hardware_cursor(tty_column, tty_row);
}

void tty_set_cursor(size_t x, size_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        tty_column = x;
        tty_row = y;
        set_hardware_cursor(tty_column, tty_row);
    }
}

void tty_end_line(void) {
    tty_column = 0;
    if (++tty_row == VGA_HEIGHT) {
        // Scroll up instead of wrapping to top
        tty_scroll();
        tty_row = VGA_HEIGHT - 1; // Stay at bottom row
    }
    set_hardware_cursor(tty_column, tty_row);
}

void tty_init(void) {
    tty_row = 0;
    tty_column = 0;
    tty_color = vga_entry_color(PRINT_COLOR_DEFAULT, TTY_BACKGROUND_COLOR);
    tty_buffer = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            tty_buffer[index] = vga_entry(' ', tty_color);
        }
    }
    set_cursor_visibility(1);
    set_hardware_cursor(tty_column, tty_row);
}

void tty_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            tty_buffer[index] = vga_entry(' ', tty_color);
        }
    }
}

void tty_clear_row(size_t row) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = row * VGA_WIDTH + x;
        tty_buffer[index] = vga_entry(' ', tty_color);
    }
}

void tty_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t src_index = y * VGA_WIDTH + x;
            const size_t dst_index = (y - 1) * VGA_WIDTH + x;
            tty_buffer[dst_index] = tty_buffer[src_index];
        }
    }
    tty_clear_row(VGA_HEIGHT - 1);
}

void tty_setcolor(uint8_t color) {
    tty_color = color;
}

void tty_putchar_at(unsigned char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    tty_buffer[index] = vga_entry(c, color);
}

void tty_putchar(char c) {
    unsigned char uc = c;
    tty_putchar_at(uc, tty_color, tty_column, tty_row);
    if (++tty_column == VGA_WIDTH) {
        tty_column = 0;
        if (++tty_row == VGA_HEIGHT) {
            tty_scroll();
            tty_row = VGA_HEIGHT - 1;
        }
    }
    set_hardware_cursor(tty_column, tty_row);
}

void tty_putstr(const char* data) {
    for (int i = 0; i < strlen(data); i++)
        tty_putchar(data[i]);
}

void tty_putstr_endl(const char* data) {
    tty_putstr(data);
    tty_end_line();
}

void tty_middle_screen(const char* data, size_t len) {
    size_t x = (VGA_WIDTH - len) / 2;
    size_t y = VGA_HEIGHT / 2;
    for (int i = 0; i < len; i++)
        tty_putchar_at(data[i], tty_color, x + i, y);
}

void tty_enable_cursor(void) {
    set_cursor_visibility(1);
}

void tty_disable_cursor(void) {
    set_cursor_visibility(0);
}

void welcome_message(void) {
    tty_setcolor(PRINT_COLOR_DANOS);
    tty_putstr_endl("    ____                    ____  _____");
    tty_putstr_endl("   / __ \\____ _____        / __ \\/ ___/");
    tty_putstr_endl("  / / / / __ `/ __ \\______/ / / /\\__ \\");
    tty_putstr_endl(" / /_/ / /_/ / / / /_____/ /_/ /___/ /");
    tty_putstr_endl("/_____/\\__,_/_/ /_/      \\____//____/");
    tty_putstr_endl("");
    tty_putstr_endl("version 0.1.1");
    tty_putstr_endl("created by dan13615");
    tty_putstr_endl("");
    tty_setcolor(PRINT_COLOR_DEFAULT);
}

void print_prompt(void) {
    tty_setcolor(PRINT_COLOR_PROMPT);
    tty_putstr(SHELL_PROMPT);
    tty_setcolor(PRINT_COLOR_DEFAULT);
}

void print_default(const char* input_buffer) {
    tty_setcolor(PRINT_COLOR_DEFAULT);
    tty_putstr_endl(input_buffer);
}

void putstr_error(const char* input_buffer) {
    tty_setcolor(PRINT_COLOR_ERROR);
    tty_putstr(input_buffer);
    tty_setcolor(PRINT_COLOR_DEFAULT);
}

void print_error(const char* input_buffer) {
    tty_setcolor(PRINT_COLOR_ERROR);
    tty_putstr_endl(input_buffer);
    tty_setcolor(PRINT_COLOR_DEFAULT);
}
