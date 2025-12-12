# TinyOS - Simple Operating System

OS sederhana dengan CLI dan filesystem untuk pembelajaran.

## 📋 Fitur

- ✅ Bootloader (real mode → protected mode)
- ✅ VGA Text Driver (80x25)
- ✅ Keyboard Input
- ✅ In-Memory Filesystem
- ✅ Command Line Interface

## 🚀 Quick Start

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install nasm gcc qemu-system-x86 build-essential
```

**Arch Linux:**
```bash
sudo pacman -S nasm gcc qemu-system-x86
```

### 2. Setup Project

```bash
# Buat folder structure
mkdir -p tinyos/{drivers,fs,shell,lib}
cd tinyos

# Copy semua file ke folder masing-masing:
# - Root: boot.asm, kernel_entry.asm, kernel.c, link.ld, Makefile, README.md
# - drivers/: vga.h, vga.c, keyboard.h, keyboard.c
# - fs/: fs.h, fs.c
# - shell/: shell.h, shell.c
# - lib/: string.h, string.c
```

### 3. Build & Run

```bash
# Build OS
make

# Run di QEMU
make run

# Clean build files
make clean
```

## 📁 Struktur File

```
tinyos/
├── boot.asm              # Bootloader
├── kernel_entry.asm      # Kernel entry
├── kernel.c              # Kernel main
├── link.ld               # Linker script
├── Makefile              # Build system
├── drivers/
│   ├── vga.h             # VGA header
│   ├── vga.c             # VGA driver
│   ├── keyboard.h        # Keyboard header
│   └── keyboard.c        # Keyboard driver
├── fs/
│   ├── fs.h              # Filesystem header
│   └── fs.c              # Filesystem impl
├── shell/
│   ├── shell.h           # Shell header
│   └── shell.c           # Shell impl
└── lib/
    ├── string.h          # String header
    └── string.c          # String utilities
```

## 💻 Commands

| Command | Deskripsi |
|---------|-----------|
| `help` | Tampilkan daftar command |
| `clear` | Clear screen |
| `ls` | List files dan folder |
| `pwd` | Print working directory |
| `cd <path>` | Change directory |
| `mkdir <name>` | Buat folder |
| `touch <file>` | Buat file kosong |
| `cat <file>` | Tampilkan isi file |
| `echo text > file` | Tulis text ke file |
| `write <file>` | Edit file (ketik :wq untuk save) |
| `rm <name>` | Hapus file/folder |
| `tree` | Tampilkan struktur folder |
| `reboot` | Reboot system |

## 📝 Contoh Penggunaan

```bash
# Buat folder
tinyos:/> mkdir documents

# Pindah ke folder
tinyos:/> cd documents

# Buat file
tinyos:/documents> touch hello.txt

# Tulis ke file
tinyos:/documents> echo Hello World! > hello.txt

# Baca file
tinyos:/documents> cat hello.txt
Hello World!

# List files
tinyos:/documents> ls
hello.txt

# Kembali
tinyos:/documents> cd ..

# Tampilkan tree
tinyos:/> tree
/
  documents/
    hello.txt
```

## 🔧 Troubleshooting

**Build error:**
```bash
make clean
make
```

**Keyboard tidak respon:**
- Klik window QEMU untuk focus

**QEMU tidak ada:**
```bash
sudo apt install qemu-system-x86
```

## 📖 Cara Kerja

1. **Boot:** `boot.asm` load kernel dari disk ke memory
2. **Protected Mode:** Switch dari 16-bit ke 32-bit mode
3. **Kernel:** Initialize VGA, keyboard, filesystem
4. **Shell:** Loop untuk terima dan execute command

## ✅ Checklist Build

Jika berhasil:
- ✅ Muncul "✅ TinyOS built successfully!"
- ✅ File `os-image.bin` terbuat
- ✅ QEMU window muncul dengan shell prompt

---

**Dibuat untuk pembelajaran OS development**
