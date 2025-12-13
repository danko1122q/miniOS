#ifndef VGA_H
#define VGA_H

#include <stdint.h>

// Basic functions
void vga_init(void);
void vga_clear(void);
void vga_putch(char c);
void vga_puts(const char *str);
void vga_set_color(uint8_t fg, uint8_t bg);

// Cursor functions
int vga_get_cursor_col(void);
int vga_get_cursor_row(void);
void vga_set_cursor(int row, int col);

// Enhanced functions
void vga_print_int(int num);
void vga_print_hex(uint32_t num);
void vga_clear_eol(void);
void vga_draw_box(int row, int col, int width, int height, uint8_t fg,
                  uint8_t bg);

// VGA Color definitions
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_YELLOW 14
#define VGA_COLOR_WHITE 15

#endif
