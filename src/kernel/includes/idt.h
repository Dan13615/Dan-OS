/**========================================================================
 *                             idt.h
 *  Interrupt Descriptor Table (IDT) header file
 *  This file contains the definitions and declarations for the IDT.
 *  The IDT is used to handle interrupts and exceptions in the kernel.
 *========================================================================**/

#ifndef IDT_H
  #define IDT_H
    /**========================================================================
     *                             STRUCTURES AND CONSTANTS
     *  This section defines the structures and constants used for the IDT.
     *========================================================================**/
    #include <stdint.h>

    // IDT entry structure for x86_64 (16 bytes)
    struct idt_entry {
        uint16_t offset_low;    // bits 0..15 of handler address
        uint16_t selector;      // kernel segment selector
        uint8_t  ist;           // bits 0..2 hold Interrupt Stack Table offset, rest zero
        uint8_t  type_attr;     // type and attributes
        uint16_t offset_mid;    // bits 16..31 of handler address
        uint32_t offset_high;   // bits 32..63 of handler address
        uint32_t zero;          // reserved
    } __attribute__((packed));

    // IDT pointer structure for lidt in 64-bit mode (10 bytes)
    struct idt_ptr {
        uint16_t limit;       // Size of the IDT in bytes - 1
        uint64_t base;        // Base address of the first element in the IDT
    } __attribute__((packed));

    # define IDT_SIZE 256 // Specific to x86 architecture
    # define KERNEL_CODE_SEGMENT_OFFSET 0x8
    # define IDT_INTERRUPT_GATE_32BIT 0x8e

    /**========================================================================
     *                             FUNCTIONS
     *  This section declares the functions used to initialize and manage the IDT.
     *========================================================================**/

    void init_idt(void); // Initialize the IDT
#endif