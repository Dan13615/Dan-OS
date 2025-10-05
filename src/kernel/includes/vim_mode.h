/*************
 *
 *     @
 *       @
 *   @ @ @
 *
 *************/

#ifndef INCLUDED_VIM_MODE_H
#define INCLUDED_VIM_MODE_H

#include "bool.h"
#include "string.h"
#include "vga.h"
#include "tty.h"

#include <stddef.h>
#include <stdint.h>

typedef enum {
    VIM_MODE_NORMAL,
    VIM_MODE_INSERT,
    VIM_MODE_COMMAND,
} editor_mode_t;

typedef struct {
        char          buffer[VGA_HEIGHT][VGA_WIDTH];
        size_t        cursor_x;
        size_t        cursor_y;
        editor_mode_t mode;
        char          command_buffer[64];
        size_t        command_len;
} editor_state_t;

void start_vim(void);
bool is_vim_enabled();
void send_key_handle_to_vim(char);

#endif
