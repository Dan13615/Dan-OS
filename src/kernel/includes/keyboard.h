/**========================================================================
 *                             KEYBOARD DRIVER
 *  Keyboard driver header file
 *  Author: Daniil Stepanov
 *  Date: 2025-09-30
 *========================================================================**/

#ifndef KEYBOARD_H
  #define KEYBOARD_H
    /**========================================================================
     *                             IO PORTS AND CONSTANTS
     *  This section defines the I/O ports and constants used by the keyboard
     *  driver.
     *========================================================================**/
    # define IDT_SIZE 256 // Specific to x86 architecture
    # define KERNEL_CODE_SEGMENT_OFFSET 0x8
    # define IDT_INTERRUPT_GATE_32BIT 0x8e

    # define PIC1_COMMAND_PORT 0x20
    # define PIC1_DATA_PORT 0x21
    # define PIC2_COMMAND_PORT 0xA0
    # define PIC2_DATA_PORT 0xA1

    # define KEYBOARD_DATA_PORT 0x60
    # define KEYBOARD_STATUS_PORT 0x64

    extern unsigned char keyboard_map[128];

    /**========================================================================
     *                             FUNCTIONS
     *  This section defines the functions used by the keyboard driver.
     *========================================================================**/

    void handle_keyboard_interrupt(void);
    void init_keyboard(void);

#endif //KEYBOARD_H