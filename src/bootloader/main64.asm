global long_mode_start
global load_idt
global keyboard_handler
global ioport_in
global ioport_out
global enable_interrupts
global timer_handler

extern kernel_main
extern handle_keyboard_interrupt
extern handle_timer_interrupt

section .text
bits 64

load_idt:
	lidt [rdi]
	ret

enable_interrupts:
	sti
	ret

keyboard_handler:
	push rbp
	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	cld
	sub rsp, 8
	call handle_keyboard_interrupt
	add rsp, 8
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax
	pop rbp
	iretq

timer_handler:
	push rbp
	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	cld
	sub rsp, 8
	call handle_timer_interrupt
	add rsp, 8
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax
	pop rbp
	iretq

ioport_in:
	mov dx, di
	in al, dx
	movzx rax, al
	ret

ioport_out:
	mov dx, di
	mov al, sil
	out dx, al
	ret


long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	mov rsp, stack_space
	cli				; Disable interrupts
	mov rsp, stack_space

	call kernel_main
    hlt

section .bss
    align 16
    stack_space resb 8192