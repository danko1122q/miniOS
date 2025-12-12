ASM = nasm
CC = gcc
LD = ld
OBJDUMP = objdump

CFLAGS = -m32 -ffreestanding -fno-builtin -fno-pie -nostdlib -nostdinc \
         -Wall -Wextra -O2 -fno-stack-protector \
         -Idrivers -Ifs -Ishell -Ilib

LDFLAGS = -m elf_i386 -T link.ld

DRIVER_SRCS = drivers/vga.c drivers/keyboard.c
FS_SRCS = fs/fs.c
SHELL_SRCS = shell/shell.c
LIB_SRCS = lib/string.c

DRIVER_OBJS = $(DRIVER_SRCS:.c=.o)
FS_OBJS = $(FS_SRCS:.c=.o)
SHELL_OBJS = $(SHELL_SRCS:.c=.o)
LIB_OBJS = $(LIB_SRCS:.c=.o)

OBJS = kernel_entry.o kernel.o $(DRIVER_OBJS) $(FS_OBJS) $(SHELL_OBJS) $(LIB_OBJS)

# QEMU display options untuk fullscreen yang lebih baik
QEMU_OPTS = -display gtk,zoom-to-fit=on,grab-on-hover=on \
            -m 64M \
            -drive format=raw,file=os-image.bin

all: os-image.bin

# Build bootloader (512 bytes)
bootloader.bin: boot.asm
	@echo "[ASM] $<"
	@nasm -f bin $< -o $@
	@truncate -s 512 $@

# kernel entry
kernel_entry.o: kernel_entry.asm
	@echo "[ASM] $<"
	@nasm -f elf32 $< -o $@

# C -> objects
%.o: %.c
	@echo "[CC]  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link kernel (binary)
kernel.bin: $(OBJS) link.ld
	@echo "[LD]  kernel.bin"
	@$(LD) $(LDFLAGS) $(OBJS) -o $@

# final image: bootloader + kernel
os-image.bin: bootloader.bin kernel.bin
	@cat bootloader.bin kernel.bin > $@
	@echo ""
	@echo "====================================="
	@echo "✅ MiniOS built successfully!"
	@echo "====================================="
	@ls -lh os-image.bin
	@echo "====================================="
	@echo "Commands:"
	@echo "  make run       - Run in window mode"
	@echo "  make fullscreen - Run in fullscreen"
	@echo "  make debug     - Run with debugger"
	@echo ""

run: os-image.bin
	@qemu-system-i386 $(QEMU_OPTS)

fullscreen: os-image.bin
	@qemu-system-i386 $(QEMU_OPTS) -full-screen

debug: os-image.bin
	@qemu-system-i386 $(QEMU_OPTS) -s -S

clean:
	@echo "Cleaning..."
	@rm -f *.o *.bin os-image.bin
	@rm -f drivers/*.o fs/*.o shell/*.o lib/*.o
	@echo "Done!"

.PHONY: all run fullscreen debug clean
