; kernel_entry.asm
bits 32
extern kernel_main

section .text.entry
global kernel_entry

kernel_entry:
    ; set up segments and stack (protected mode)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00

    ; call C kernel entry
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
