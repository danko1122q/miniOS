#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t *)0xB8000)

static uint8_t vga_color = (VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4));
static int cursor_row = 0;
static int cursor_col = 0;

static inline void outb(uint16_t port, uint8_t value)
{
	__asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint16_t vga_entry(char c, uint8_t color)
{
	return (uint16_t)c | ((uint16_t)color << 8);
}

static void update_cursor(void)
{
	uint16_t pos = cursor_row * VGA_WIDTH + cursor_col;
	outb(0x3D4, 14);
	outb(0x3D5, (pos >> 8) & 0xFF);
	outb(0x3D4, 15);
	outb(0x3D5, pos & 0xFF);
}

void vga_set_color(uint8_t fg, uint8_t bg)
{
	vga_color = fg | (bg << 4);
}

void vga_clear(void)
{
	for (int y = 0; y < VGA_HEIGHT; y++) {
		for (int x = 0; x < VGA_WIDTH; x++) {
			VGA_MEMORY[y * VGA_WIDTH + x] =
			    vga_entry(' ', vga_color);
		}
	}
	cursor_row = 0;
	cursor_col = 0;
	update_cursor();
}

void vga_init(void)
{
	vga_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);
	vga_clear();
}

void vga_putch(char c)
{
	if (c == '\b') {
		// BACKSPACE - FIX FINAL
		if (cursor_col > 0) {
			cursor_col--;
			// Tulis spasi untuk menghapus karakter
			VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] =
			    vga_entry(' ', vga_color);
			update_cursor();
		}
		return;
	}

	if (c == '\n') {
		cursor_col = 0;
		cursor_row++;
	} else if (c >= 32 && c <= 126) {
		VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] =
		    vga_entry(c, vga_color);
		cursor_col++;

		if (cursor_col >= VGA_WIDTH) {
			cursor_col = 0;
			cursor_row++;
		}
	}

	if (cursor_row >= VGA_HEIGHT) {
		// Scroll up
		for (int y = 0; y < VGA_HEIGHT - 1; y++) {
			for (int x = 0; x < VGA_WIDTH; x++) {
				VGA_MEMORY[y * VGA_WIDTH + x] =
				    VGA_MEMORY[(y + 1) * VGA_WIDTH + x];
			}
		}
		// Clear last line
		for (int x = 0; x < VGA_WIDTH; x++) {
			VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
			    vga_entry(' ', vga_color);
		}
		cursor_row = VGA_HEIGHT - 1;
	}

	update_cursor();
}

void vga_puts(const char *str)
{
	while (*str) {
		vga_putch(*str++);
	}
}

void vga_print_int(int num)
{
	if (num == 0) {
		vga_putch('0');
		return;
	}

	if (num < 0) {
		vga_putch('-');
		num = -num;
	}

	char buffer[12];
	int i = 0;

	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}

	while (i > 0) {
		vga_putch(buffer[--i]);
	}
}

void vga_print_hex(uint32_t num)
{
	vga_puts("0x");
	const char hex[] = "0123456789ABCDEF";
	for (int i = 28; i >= 0; i -= 4) {
		vga_putch(hex[(num >> i) & 0xF]);
	}
}

int vga_get_cursor_col(void)
{
	return cursor_col;
}

int vga_get_cursor_row(void)
{
	return cursor_row;
}

void vga_set_cursor(int row, int col)
{
	cursor_row = row;
	cursor_col = col;
	update_cursor();
}

void vga_clear_eol(void)
{
	for (int x = cursor_col; x < VGA_WIDTH; x++) {
		VGA_MEMORY[cursor_row * VGA_WIDTH + x] =
		    vga_entry(' ', vga_color);
	}
}

void vga_draw_box(int row, int col, int width, int height, uint8_t fg,
                  uint8_t bg)
{
	uint8_t old_color = vga_color;
	vga_color = fg | (bg << 4);

	vga_set_cursor(row, col);
	vga_putch('+');
	for (int i = 0; i < width - 2; i++)
		vga_putch('-');
	vga_putch('+');

	for (int y = 1; y < height - 1; y++) {
		vga_set_cursor(row + y, col);
		vga_putch('|');
		vga_set_cursor(row + y, col + width - 1);
		vga_putch('|');
	}

	vga_set_cursor(row + height - 1, col);
	vga_putch('+');
	for (int i = 0; i < width - 2; i++)
		vga_putch('-');
	vga_putch('+');

	vga_color = old_color;
}
