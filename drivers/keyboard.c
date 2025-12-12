#include "keyboard.h"
#include "vga.h"

// Scancode to ASCII mapping (without shift)
static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '
};

// Scancode to ASCII mapping (with shift)
static const char scancode_to_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,  ' '
};

static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;

#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_LCTRL  0x1D
#define KEY_RELEASE 0x80

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void keyboard_init(void) {
    while (inb(0x64) & 1) inb(0x60);
}

int keyboard_has_input(void) {
    return (inb(0x64) & 1);
}

char keyboard_getchar(void) {
    while (1) {
        if (!keyboard_has_input()) continue;
        
        uint8_t scancode = inb(0x60);
        
        // Handle key release
        if (scancode & KEY_RELEASE) {
            scancode &= ~KEY_RELEASE;
            if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
                shift_pressed = 0;
            }
            if (scancode == KEY_LCTRL) {
                ctrl_pressed = 0;
            }
            continue;
        }
        
        // Handle shift press
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 1;
            continue;
        }
        
        // Handle ctrl press
        if (scancode == KEY_LCTRL) {
            ctrl_pressed = 1;
            continue;
        }
        
        // Get character
        if (scancode < 128) {
            char c;
            if (shift_pressed) {
                c = scancode_to_ascii_shift[scancode];
            } else {
                c = scancode_to_ascii[scancode];
            }
            
            // Handle Ctrl combinations
            if (ctrl_pressed && c >= 'a' && c <= 'z') {
                return c - 'a' + 1; // Ctrl+A = 1, Ctrl+B = 2, etc.
            }
            
            if (c) return c;
        }
    }
}

char keyboard_getchar_with_ctrl(void) {
    return keyboard_getchar();
}

int keyboard_ctrl_pressed(void) {
    return ctrl_pressed;
}

void keyboard_readline(char* buffer, int max_len) {
    int pos = 0;
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            buffer[pos] = '\0';
            vga_putch('\n');
            return;
        }
        
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                vga_putch('\b');
            }
            continue;
        }
        
        if (pos < max_len - 1 && c >= 32 && c <= 126) {
            buffer[pos++] = c;
            vga_putch(c);
        }
    }
}
