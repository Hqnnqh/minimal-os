#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} vga_color;
	
static inline uint8_t vga_entry_color(vga_color fg, vga_color bg) {
	return fg | bg << 4;	
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return color << 8 | (uint16_t) uc;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

void monitor_scroll(void); // function prototype

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_selector(uint8_t color) {
	terminal_color = color;
}

void terminal_put_entry_at(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_put_char(char c) {
	if (c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) monitor_scroll();
		return;
	}
	
	terminal_put_entry_at(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) monitor_scroll();	
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) terminal_put_char(data[i]);	
}

void terminal_write_string(const char* data) {
	terminal_write(data, strlen(data));
}


void monitor_scroll(void) {
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			// Copy the current row to the previous row
			const size_t index = (y - 1) * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_buffer[y * VGA_WIDTH + x];
		}
	}
	// clear last row
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
	}
	terminal_row = VGA_HEIGHT - 1;
}

void kernel_main(void) {
	terminal_initialize();
	terminal_write_string("Hello World!\n:)\n");
}
