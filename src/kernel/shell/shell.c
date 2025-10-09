/**========================================================================
 *                             SHELL MODULE
 *  description: This module implements a simple shell interface for the OS.
 *  It provides basic functionalities such as command execution, input
 *  handling, and output display.
 *  author: Daniil Stepanov
 *  date: 2025-10-02
 *========================================================================**/

#include "shell.h"
#include "tty.h"
#include "string.h"
#include "bool.h"

static char input_buffer[SHELL_MAX_INPUT_LENGTH];
static size_t buffer_index;

void init_shell(void) {
    clear_buffer();
    buffer_index = 0;
    print_prompt();
}

void add_to_buffer(char c) {
    bool new_line = true;

    if (c == '\b') { // Handle backspace
        if (buffer_index > 0) {
            buffer_index--;
            input_buffer[buffer_index] = '\0';
            tty_cursor_backspace(1);
        }
    } else if (c == '\n') { // Handle enter key
        input_buffer[buffer_index] = '\0';
        if (buffer_index > 0) {
            tty_end_line();
            tty_putstr("> ");
            if (process_command(input_buffer, buffer_index) == 1) {
                new_line = false;
            }
        }
        clear_buffer();
        if (new_line) tty_end_line();
        print_prompt(); // Display prompt again
    } else if (buffer_index < SHELL_MAX_INPUT_LENGTH - 1) { // Regular character
        tty_putchar(c);
        input_buffer[buffer_index++] = c;
        input_buffer[buffer_index] = '\0';
    }
}

void clear_buffer(void) {
    memset(input_buffer, 0, sizeof(input_buffer));
    buffer_index = 0;
}