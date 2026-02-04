//
// Created by dan13615 on 11/15/24.
//

#include "tty.h"
#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include "timer.h"
#include "fat32.h"

extern void enable_interrupts();

void kernel_main(void) {
    tty_init();
    init_pit(1000);
    welcome_message();
    init_idt();
    init_keyboard();
    init_shell();
    enable_interrupts();
    fat32_init(2048);
    while(1){}
}