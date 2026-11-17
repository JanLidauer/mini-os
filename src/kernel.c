#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
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
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/* schiebt alle zeilen um eins nach oben und leert die letzte zeile */
static void terminal_scroll(void)
{
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++)
			terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
	}
	for (size_t x = 0; x < VGA_WIDTH; x++)
		terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
}

/* geht in die naechste zeile, scrollt wenn der bildschirm voll ist */
static void terminal_newline(void)
{
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		terminal_scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}

void terminal_putchar(char c) 
{
	if (c == '\n') {
		terminal_newline();
		return;
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH)
		terminal_newline();
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

/* zeigt den timer-zaehler oben rechts an: beweist, dass hardware-irqs ankommen.
   schreibt direkt an feste position, ohne den cursor des terminals anzufassen */
static void show_timer_ticks(void)
{
	uint32_t n = timer_ticks;
	size_t x = VGA_WIDTH - 1;

	do {
		terminal_putentryat('0' + (n % 10), vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK), x--, 0);
		n /= 10;
	} while (n > 0);
}

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();

	gdt_install();
	idt_install();
	irq_install();   /* pic remappen: hardware-irqs auf vektor 32-47 */
	timer_install(); /* irq 0 mitzaehlen lassen */

	terminal_writestring("Hello, kernel World!\n");
	terminal_writestring("GDT, IDT und PIC aktiv, Interrupts laufen.\n");

	/* IDT Test: Testweise eine Exception ausloesen (Breakpoint, index 3)*/
	// asm volatile("int $0x3");

	/* ab hier duerfen hardware-interrupts ankommen. gdt, idt und pic muessen davor stehen */
	asm volatile("sti");

	/* leerlauf: hlt schlaeft bis zum naechsten interrupt (timer, spaeter tastatur).
	   kernel_main darf nicht zurueckkehren, weil boot.s danach mit cli stehen bleibt */
	for (;;) {
		asm volatile("hlt");
		show_timer_ticks();
	}
}
