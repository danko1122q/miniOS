#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "fs/fs.h"
#include "shell/shell.h"

void kernel_main(void) {
    vga_init();
    vga_clear();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("Booting MiniOS...\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    vga_puts("Initializing keyboard... ");
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("[OK]\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    vga_puts("Initializing filesystem... ");
    fs_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("[OK]\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    vga_putch('\n');
    
    // Clear screen before starting shell
    vga_clear();
    
    shell_init();
    shell_run();
    
    for(;;) __asm__ volatile("hlt");
}
