; ---------------------------------------------------------
; Bootloader untuk TinyOS - Load kernel dan masuk protected mode
; File: boot.asm
; ---------------------------------------------------------

[org 0x7C00]
[bits 16]

KERNEL_OFFSET equ 0x1000        ; Kernel di-load ke sini
KERNEL_SECTORS equ 20           ; Jumlah sector kernel (sesuaikan jika perlu)

start:
    ; Setup segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Print loading message
    mov si, msg_loading
    call print_string

    ; Load kernel dari disk
    call load_kernel

    ; Print entering protected mode message
    mov si, msg_protected
    call print_string

    ; Masuk protected mode
    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    jmp CODE_SEG:init_pm

; ---------------------------------------------------------
; Load kernel dari disk ke memory
; ---------------------------------------------------------
load_kernel:
    mov ah, 0x02                ; BIOS read sectors
    mov al, KERNEL_SECTORS      ; Jumlah sectors
    mov ch, 0                   ; Cylinder 0
    mov cl, 2                   ; Start dari sector 2 (setelah bootloader)
    mov dh, 0                   ; Head 0
    mov dl, 0x80                ; First hard disk
    mov bx, KERNEL_OFFSET       ; ES:BX = destinasi buffer
    int 0x13
    
    jc disk_error               ; Jump jika error
    ret

disk_error:
    mov si, msg_error
    call print_string
    jmp $

; ---------------------------------------------------------
; Print string (real mode)
; ---------------------------------------------------------
print_string:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa
    ret

; ---------------------------------------------------------
; Protected Mode
; ---------------------------------------------------------
[bits 32]
init_pm:
    ; Setup segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00

    ; Jump ke kernel
    call KERNEL_OFFSET

    ; Jika kernel return, hang
    jmp $

; ---------------------------------------------------------
; GDT (Global Descriptor Table)
; ---------------------------------------------------------
[bits 16]
gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_data:
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10010010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ---------------------------------------------------------
; Messages
; ---------------------------------------------------------
msg_loading db "Loading TinyOS kernel...", 13, 10, 0
msg_protected db "Entering protected mode...", 13, 10, 0
msg_error db "Disk read error!", 13, 10, 0

; ---------------------------------------------------------
; Boot signature
; ---------------------------------------------------------
times 510-($-$$) db 0
dw 0xAA55
