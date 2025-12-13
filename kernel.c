#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "fs/fs.h"
#include "shell/shell.h"

static inline uint8_t inb(uint16_t port)
{
	uint8_t value;
	__asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

static inline void outb(uint16_t port, uint8_t value)
{
	__asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t get_second(void)
{
	uint8_t sec, sec2;
	do {
		outb(0x70, 0x00);
		sec = inb(0x71);
		outb(0x70, 0x00);
		sec2 = inb(0x71);
	} while (sec != sec2);
	return sec;
}

static void delay_4_seconds(void)
{
	uint8_t start_sec = get_second();
	int count = 0;

	while (count < 4) {
		uint8_t current_sec = get_second();
		if (current_sec != start_sec) {
			count++;
			start_sec = current_sec;
		}
	}
}

void kernel_main(void)
{
	vga_init();
	vga_clear();

	// Linux-style boot messages
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	vga_puts("Booting MiniOS...\n");

	vga_puts("Initializing keyboard... ");
	keyboard_init();
	vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	vga_puts("OK\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	vga_puts("Initializing filesystem... ");
	fs_init();
	vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	vga_puts("OK\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	vga_puts("Starting system services... ");
	vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	vga_puts("OK\n\n");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
	vga_puts("Booting will continue in 4 seconds...");
	vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	delay_4_seconds();

	// Clear screen before showing welcome message
	vga_clear();

	shell_init();
	shell_run();

	for (;;)
		__asm__ volatile("hlt");
}
