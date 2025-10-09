/**========================================================================
 *                             SHELL MODULE
 *  description: This module implements a simple shell interface for the OS.
 *  It provides basic functionalities such as command execution, input
 *  handling, and output display.
 *  author: Daniil Stepanov
 *  date: 2025-10-02
 *========================================================================**/

#ifndef SHELL_H
  #define SHELL_H

    #include <stdint.h>
    #include <stddef.h>

    #define SHELL_MAX_INPUT_LENGTH 128
    // #define SHELL_MAX_ARGS 16
    #define UNUSED(x) (void)(x) // Macro to suppress unused variable warnings

    void add_to_buffer(char c);
    void clear_buffer(void);
    void init_shell(void);

  /**=======================
   *     COMMANDS
   *========================**/

    int process_command(const char* input, size_t length);
    int cmd_echo(const char* args);
    int cmd_clear(const char* args);
    int cmd_help(const char* args);

#endif //SHELL_H