#include "vim_mode.h"

static bool           _is_vim_enabled = false;
static editor_state_t state           = {0};

static uint8_t color                  = 0x07;

bool is_vim_enabled()
{
    return _is_vim_enabled;
}

static void print_data_on_bottom_right(const char* data)
{
    if (strlen(data) >= 10)
        return;
    size_t cursor_pos_x = state.cursor_x;
    size_t cursor_pos_y = state.cursor_y;

    tty_set_cursor(VGA_WIDTH - 11, VGA_HEIGHT - 1);
    tty_putstr("          ");
    tty_set_cursor(VGA_WIDTH - strlen(data) - 1, VGA_HEIGHT - 1);
    tty_putstr(data);
    tty_set_cursor(cursor_pos_x, cursor_pos_y);
}

void start_vim()
{
    _is_vim_enabled = true;
    memset(&state, 0, sizeof(editor_state_t));
    tty_clear();
    tty_set_cursor(0, 0);
    print_data_on_bottom_right("NORMAL");
    tty_enable_cursor();
}

static void handle_normal_mode(char c)
{
    switch (c) {
        case 'h':
            if (state.cursor_x > 0) state.cursor_x--;
            break;
        case 'l':
            if (state.cursor_x < VGA_WIDTH - 1) state.cursor_x++;
            break;
        case 'k':
            if (state.cursor_y > 0) state.cursor_y--;
            break;
        case 'j':
            if (state.cursor_y < VGA_HEIGHT - 1) state.cursor_y++;
            break;
        case 'i':
            state.mode = VIM_MODE_INSERT;
            print_data_on_bottom_right("INSERT");
            break;
        // This custom implementation of a really basic vim like text editor
        // uses . instead of : to be easier to use with qwerty keyboards
        case '.':
            state.mode        = VIM_MODE_COMMAND;
            state.command_len = 0;
            print_data_on_bottom_right("COMMAND");
            tty_set_cursor(0, VGA_HEIGHT - 1);    // draw at bottom
            tty_putstr(":");
            return;
    }

    tty_set_cursor(state.cursor_x, state.cursor_y);
}

static void handle_insert_mode(char c)
{
    if (c == 27) {
        state.mode = VIM_MODE_NORMAL;
        print_data_on_bottom_right("NORMAL");
        return;
    }


    if (c == '\b') {
        if (state.cursor_x > 0) {
            state.cursor_x--;
            state.buffer[state.cursor_y][state.cursor_x] = ' ';
            tty_putchar_at(' ', color, state.cursor_x, state.cursor_y);
        }
        return;
    }

    state.buffer[state.cursor_y][state.cursor_x] = c;
    tty_putchar_at(c, color, state.cursor_x, state.cursor_y);

    if (state.cursor_x < VGA_WIDTH - 1) state.cursor_x++;
}

static void handle_command_mode(char c)
{
    if (c == '\n') {
        state.command_buffer[state.command_len] = '\0';

        if (strcmp(state.command_buffer, "q") == 0) {
            _is_vim_enabled = false;
            tty_clear();
            tty_set_cursor(0, 0);
            print_prompt();
            return;    // exit editor
            // } else if (strcmp(state.command_buffer, "w") == 0) {
            //     // write to file
            // } else if (strcmp(state.command_buffer, "wq") == 0) {
            //     // write to file
            //     tty_clear();
            //     return;
            // } else {
            //     tty_putstr_endl("Unkown command");
        }

        state.mode = VIM_MODE_NORMAL;
        print_data_on_bottom_right("NORMAL");
        return;
    }

    if (c == '\b' && state.command_len > 0) {
        state.command_len--;
        tty_cursor_backspace(1);
        return;
    }

    if (state.command_len < sizeof(state.command_buffer) - 1) {
        state.command_buffer[state.command_len++] = c;
        tty_putchar(c);
    }
}

void send_key_handle_to_vim(char c)
{
    switch (state.mode) {
        case VIM_MODE_NORMAL:  handle_normal_mode(c); break;
        case VIM_MODE_INSERT:  handle_insert_mode(c); break;
        case VIM_MODE_COMMAND: handle_command_mode(c); break;
    }
}
