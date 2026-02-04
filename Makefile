ASM_SRCS 	=	$(wildcard src/bootloader/*.asm)

KERNEL_SRCS =	$(wildcard src/kernel/*.c)
TTY_SRCS 	=	$(wildcard src/kernel/tty/*.c)
STR_SRCS 	=	$(wildcard src/kernel/string/*.c)
IDT_SRCS 	=	$(wildcard src/kernel/idt/*.c)
KEYBOARD_SRCS =	$(wildcard src/kernel/keyboard/*.c)
TIMER_SRCS  =   $(wildcard src/kernel/timer/*.c)
SHELL_SRCS  =   $(wildcard src/kernel/shell/*.c)
POWER_SRCS  =   $(wildcard src/kernel/power/*.c)
DISK_SRCS  	=   $(wildcard src/kernel/disk/*.c)

ASM_PATH 	= 	src/bootloader/%.asm
KERNEL_PATH = 	src/kernel/%.c
TTY_PATH 	= 	src/kernel/tty/%.c
STR_PATH 	= 	src/kernel/string/%.c
IDT_PATH 	= 	src/kernel/idt/%.c
KEYBOARD_PATH = src/kernel/keyboard/%.c
TIMER_PATH  =   src/kernel/timer/%.c
SHELL_PATH  =   src/kernel/shell/%.c
POWER_PATH  =   src/kernel/power/%.c
DISK_PATH 	=   src/kernel/disk/%.c

OBJ_PATH 	= 	obj/%.o

ASM_OBJS 	= 	$(patsubst $(ASM_PATH), $(OBJ_PATH), $(ASM_SRCS))
KERNEL_OBJS = 	$(patsubst $(KERNEL_PATH), $(OBJ_PATH), $(KERNEL_SRCS))
TTY_OBJS 	= 	$(patsubst $(TTY_PATH), $(OBJ_PATH), $(TTY_SRCS))
IDT_OBJS 	= 	$(patsubst $(IDT_PATH), $(OBJ_PATH), $(IDT_SRCS))
KEYBOARD_OBJS = $(patsubst $(KEYBOARD_PATH), $(OBJ_PATH), $(KEYBOARD_SRCS))
STR_OBJS 	= 	$(patsubst $(STR_PATH), $(OBJ_PATH), $(STR_SRCS))
TIMER_OBJS  =   $(patsubst $(TIMER_PATH), $(OBJ_PATH), $(TIMER_SRCS))
SHELL_OBJS  =   $(patsubst $(SHELL_PATH), $(OBJ_PATH), $(SHELL_SRCS))
POWER_OBJS  =   $(patsubst $(POWER_PATH), $(OBJ_PATH), $(POWER_SRCS))
DISK_OBJS  	=   $(patsubst $(DISK_PATH), $(OBJ_PATH), $(DISK_SRCS))

OBJS 		= 	$(ASM_OBJS) $(KERNEL_OBJS) $(TTY_OBJS) $(IDT_OBJS) $(KEYBOARD_OBJS) $(STR_OBJS) $(TIMER_OBJS) $(SHELL_OBJS) $(POWER_OBJS) $(DISK_OBJS)

NAME 		= 	DanOs
BIN 		= 	target/x86_64/iso/boot/kernel.bin
LINKER 		= 	src/linker/linker.ld
ISO 		= 	build/x86_64/$(NAME).iso
ISO_TARGET 	= 	target/x86_64/iso
BUILD 		= 	build/x86_64
TRASH 		= 	obj build $(BIN)
INCLUDE 	= 	src/kernel/includes
FLAGS       =   -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs

MK 			= 	mkdir -p
CC 			= 	x86_64-elf-gcc -ffreestanding -I $(INCLUDE) $(FLAGS)
NASM 		= 	nasm -f elf64
LD 			= 	x86_64-elf-ld -n -o $(BIN) -T $(LINKER) $(OBJS)
GRUB 		= 	grub-mkrescue /usr/lib/grub/i386-pc -o $(ISO) $(ISO_TARGET)
RM 			= 	rm -rf

$(STR_OBJS): $(OBJ_PATH): $(STR_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(STR_PATH), $@) -o $@

$(KERNEL_OBJS): $(OBJ_PATH): $(KERNEL_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(KERNEL_PATH), $@) -o $@

$(TTY_OBJS): $(OBJ_PATH): $(TTY_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(TTY_PATH), $@) -o $@

$(IDT_OBJS): $(OBJ_PATH): $(IDT_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(IDT_PATH), $@) -o $@

$(KEYBOARD_OBJS): $(OBJ_PATH): $(KEYBOARD_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(KEYBOARD_PATH), $@) -o $@

$(TIMER_OBJS): $(OBJ_PATH): $(TIMER_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(TIMER_PATH), $@) -o $@

$(SHELL_OBJS): $(OBJ_PATH): $(SHELL_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(SHELL_PATH), $@) -o $@

$(POWER_OBJS): $(OBJ_PATH): $(POWER_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(POWER_PATH), $@) -o $@

$(DISK_OBJS): $(OBJ_PATH): $(DISK_PATH)
	@ $(MK) $(dir $@) && \
	$(CC) -c $(patsubst $(OBJ_PATH), $(DISK_PATH), $@) -o $@

$(ASM_OBJS): $(OBJ_PATH): $(ASM_PATH)
	@ $(MK) $(dir $@) && \
	$(NASM) $(patsubst $(OBJ_PATH), $(ASM_PATH), $@) -o $@

.PHONY: build
build: $(OBJS)
	@ $(MK) $(BUILD) && \
	$(LD) && \
	$(GRUB)

.PHONY: clean
clean:
	@ $(RM) $(TRASH)

.PHONY: re
re: clean build

.PHONY: run
run:
	@ qemu-system-x86_64 -cdrom $(ISO)

.PHONY: help
help:
	@ echo "Usage: make [target]"
	@ echo "Targets:"
	@ echo "  build: Build the kernel"
	@ echo "  clean: Clean the build"
	@ echo "  re: Clean and build the kernel"
	@ echo "  run: Run the kernel"
	@ echo "  help: Show this message"

