//
// Created by dan13615 on 11/15/24.
//

#ifndef PRINT_H
	#define PRINT_H

	#include <stddef.h>
	#include <stdint.h>

    #define SHELL_PROMPT "Dan-OS &> "

	void tty_init(void);
    void tty_clear(void);
	void tty_clear_row(size_t row);
	void tty_scroll(void);
	void tty_setcolor(uint8_t color);
    void tty_putchar(char c);
	void tty_putstr(const char* data);
	void tty_putstr_endl(const char* data);
    void tty_putchar_at(unsigned char c, uint8_t color, size_t x, size_t y);
    void tty_middle_screen(const char* data, size_t len);
	void tty_cursor_backspace(size_t count);
	void tty_set_cursor(size_t x, size_t y);
	void tty_end_line(void);

	// Cursor control functions
	void tty_enable_cursor(void);
	void tty_disable_cursor(void);

	void print_prompt(void);
	void print_default(const char* input_buffer);
	void print_error(const char* input_buffer);
	void putstr_error(const char* input_buffer);
	void welcome_message(void);

#endif //PRINT_H
