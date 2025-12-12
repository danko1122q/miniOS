# MiniOS

A simple 32-bit operating system written in C and x86 assembly. Built for learning OS development fundamentals.

## What is this?

This is a basic operating system that boots from scratch and runs a simple shell with a filesystem. It's small enough to understand but functional enough to be useful for learning.

## Features

- Custom bootloader (real mode → protected mode)
- VGA text mode driver (80x25)
- PS/2 keyboard driver with Shift/Ctrl support
- In-memory filesystem (files & directories)
- Simple shell with Unix-like commands
- Basic text editor (Ctrl+S to save, Ctrl+Q to quit)

## Building

### Prerequisites

You need to install the following tools:

**On Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install nasm gcc make qemu-system-x86 binutils
```

**On Arch Linux:**
```bash
sudo pacman -S nasm gcc make qemu-system-x86 binutils
```

**On Fedora:**
```bash
sudo dnf install nasm gcc make qemu-system-x86 binutils
```

**On macOS:**
```bash
brew install nasm gcc make qemu
```

**On Windows (WSL2):**
```bash
# First install WSL2 with Ubuntu, then:
sudo apt update
sudo apt install nasm gcc make qemu-system-x86 binutils
```

### Required Tools:
- `nasm` - assembler for boot.asm and kernel_entry.asm
- `gcc` - C compiler (32-bit support required)
- `ld` - linker from binutils
- `make` - build automation tool
- `qemu-system-i386` - x86 emulator for testing

### Build Commands

```bash
make           # compile everything
make run       # run in QEMU window
make fullscreen # run in QEMU fullscreen
make clean     # cleanup
```

The build creates `os-image.bin` which is a raw disk image.

## Running

### QEMU (recommended)
```bash
make run
```

### Real Hardware (USB boot)
```bash
sudo dd if=os-image.bin of=/dev/sdX bs=512
# Replace sdX with your USB device
# WARNING: This will erase your USB drive
```

**Note**: Testing on real hardware is optional and for advanced users only. QEMU is the recommended and safest way to run MiniOS.

## Shell Commands

```
help          - show available commands
clear         - clear screen
ls            - list files in current directory
pwd           - show current directory
cd <dir>      - change directory (cd .. for parent)
mkdir <name>  - create directory
touch <file>  - create empty file
cat <file>    - display file contents (q to exit)
echo <text> > <file> - write text to file
write <file>  - edit file (Ctrl+S save, Ctrl+Q quit)
rm <name>     - remove file or directory
tree          - show directory tree
info          - system information
reboot        - reboot system
```

## Example Usage

```bash
# Create a directory
minios:/$ mkdir docs

# Go into it
minios:/$ cd docs

# Create a file
minios:/docs$ touch hello.txt

# Write to it
minios:/docs$ echo Hello World > hello.txt

# Read it
minios:/docs$ cat hello.txt

# Edit it properly
minios:/docs$ write hello.txt
# Type something, then Ctrl+S to save

# Go back
minios:/docs$ cd ..

# See the tree
minios:/$ tree
```

## How It Works

### Boot Process
1. BIOS loads first 512 bytes (bootloader) to 0x7C00
2. Bootloader loads kernel sectors from disk
3. Switches CPU to protected mode
4. Jumps to kernel entry point
5. Kernel initializes VGA, keyboard, filesystem
6. Starts shell

### Memory Layout
```
0x00000000 - 0x000003FF : Real Mode IVT
0x00000400 - 0x000004FF : BIOS Data Area
0x00000500 - 0x00007BFF : Free (30KB)
0x00007C00 - 0x00007DFF : Bootloader (512B)
0x00007E00 - 0x0007FFFF : Free (480KB)
0x00080000 - 0x0009FFFF : Extended BIOS Data Area
0x000A0000 - 0x000BFFFF : Video RAM
0x000C0000 - 0x000FFFFF : BIOS ROM
0x00100000+             : Kernel loaded here
```

### Filesystem Structure
Simple in-memory tree structure:
- Each inode has name, type (file/dir), size, data buffer
- Max 64 files/directories
- Max 4KB per file
- No persistence (RAM only)

## Learning Resources

This OS was built by learning from:

- **OSDev Wiki** (https://wiki.osdev.org/)
  - Main resource for OS development
  - Boot process, VGA, keyboard, memory management
  
- **Writing a Simple Operating System from Scratch** by Nick Blundell
  - Great tutorial on bootloader and protected mode
  - PDF available online
  
- **The Little OS Book** (https://littleosbook.github.io/)
  - Step-by-step OS development guide
  - Covers bootloader, printing, segmentation
  
- **JamesM's Kernel Tutorial** (http://jamesmolloy.co.uk/tutorial_html/)
  - Classic tutorial for basic kernel
  - Filesystem concepts
  
- **os-tutorial by cfenollosa** (https://github.com/cfenollosa/os-tutorial)
  - GitHub repo with incremental lessons
  - Great for beginners
  
## Project Structure

```
.
├── boot.asm              # Bootloader (16/32-bit assembly)
├── kernel_entry.asm      # Kernel entry point (32-bit assembly)
├── kernel.c              # Kernel main function
├── link.ld               # Linker script
├── Makefile              # Build system
├── drivers/
│   ├── vga.c/h          # VGA text mode driver
│   └── keyboard.c/h     # PS/2 keyboard driver
├── fs/
│   └── fs.c/h           # In-memory filesystem
├── shell/
│   └── shell.c/h        # Command shell
└── lib/
    ├── string.c/h       # String functions (strlen, strcmp, etc.)
    └── stdint.h         # Standard integer types
```

## Limitations

- No multitasking (single process only)
- No memory management (no malloc/free)
- No filesystem persistence (RAM only)
- No network stack
- No USB support
- Text mode only (no graphics)
- PS/2 keyboard only (no USB keyboards without legacy support)
- 32-bit only (no x64)

## Why These Limitations?

This is intentionally simple. The goal is to understand OS fundamentals without getting lost in complexity. Real operating systems have:
- Memory paging and virtual memory
- Process scheduling and multitasking
- Interrupt handling and system calls
- Device drivers for modern hardware
- Network protocols
- Filesystem with disk I/O

Adding these would make the codebase 100x larger and harder to learn from.

## Next Features

Some things you could try adding to MiniOS:

1. More CLI commands (like `cp`, `mv`, `find`)
2. A better way to manage memory
3. Simple multitasking
4. A slightly better filesystem



## Contributing

This is an educational project. Feel free to fork and experiment. If you find bugs or have improvements, pull requests are welcome.

## License

MIT License - do whatever you want with this code.

## Author

**davanico (danko1122)**

Built for learning purposes. Based on tutorials and documentation from the OSDev community.

---

**Note**: This is not a production OS. It's a learning tool. Don't run important things on it. Actually, there's nothing to run anyway since it has no programs except the shell.
