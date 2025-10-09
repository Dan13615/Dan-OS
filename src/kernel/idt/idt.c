/**========================================================================
 *                             idt.c
 *  This file contains the implementation of the Interrupt Descriptor Table (IDT)
 *  functions, including initialization and management of IDT entries.
 *========================================================================**/

#include "idt.h"
#include "keyboard.h"

extern void load_idt(void* idt_address);
extern void keyboard_handler();
extern void timer_handler();
extern void ioport_out(unsigned short port, unsigned char value);
struct idt_entry IDT[IDT_SIZE];

void init_idt(void)
{
	uint64_t offset = (uint64_t)keyboard_handler;

	IDT[0x21].offset_low = offset & 0xFFFF;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].ist = 0;
	IDT[0x21].type_attr = IDT_INTERRUPT_GATE_32BIT;
	IDT[0x21].offset_mid = (offset >> 16) & 0xFFFF;
	IDT[0x21].offset_high = (offset >> 32) & 0xFFFFFFFF;
	IDT[0x21].zero = 0;

	/* Timer IRQ (IRQ0 -> vector 0x20) */
	uint64_t timer_offset = (uint64_t)timer_handler;
	IDT[0x20].offset_low = timer_offset & 0xFFFF;
	IDT[0x20].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x20].ist = 0;
	IDT[0x20].type_attr = IDT_INTERRUPT_GATE_32BIT;
	IDT[0x20].offset_mid = (timer_offset >> 16) & 0xFFFF;
	IDT[0x20].offset_high = (timer_offset >> 32) & 0xFFFFFFFF;
	IDT[0x20].zero = 0;
    // Initialize the PIC (Programmable Interrupt Controller)
    // This is based on the OSDev wiki: https://wiki.osdev.org/8259_PIC
	ioport_out(PIC1_COMMAND_PORT, 0x11);
	ioport_out(PIC2_COMMAND_PORT, 0x11);
    // ICW2: Remap offset address of IDT
    // In x86 protected mode, we have to remap the PICs
    // beyond 0x20 because Intel have designated the first 32
    // interrupts as "reserved" for CPU exceptions
	ioport_out(PIC1_DATA_PORT, 0x20);
	ioport_out(PIC2_DATA_PORT, 0x28);
    // ICW3: Setup cascading
    // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    // Tell Slave PIC its cascade identity (0000 0010)
	ioport_out(PIC1_DATA_PORT, 0x04);
	ioport_out(PIC2_DATA_PORT, 0x02);
    // ICW4: Environment info
    // 8086/88 (MCS-80/85) mode
	ioport_out(PIC1_DATA_PORT, 0x1);
	ioport_out(PIC2_DATA_PORT, 0x1);
	// Mask interrupts initially, then unmask timer and keyboard
	ioport_out(PIC1_DATA_PORT, 0xff);
	ioport_out(PIC2_DATA_PORT, 0xff);
	// Unmask IRQ0 (timer) and IRQ1 (keyboard) on master PIC
	ioport_out(PIC1_DATA_PORT, 0xFC); // 1111 1100 -> unmask bits 0 and 1
	ioport_out(PIC2_DATA_PORT, 0xff);
    // Now, let's create the IDT pointer
	struct idt_ptr idt_ptr;
	idt_ptr.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
	idt_ptr.base = (uint64_t) &IDT;
	// Load the IDT (load_idt expects pointer in RDI)
	load_idt(&idt_ptr);
}