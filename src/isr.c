#include "isr.h"

extern void terminal_writestring(const char* data); // aus kernel.c
extern void terminal_putchar(char c);                // aus kernel.c

/* Namen der ersten 32 CPU-Exceptions fuer Ausgabe */
static const char* exception_messages[32] = {
    "Division durch Null", "Debug", "Nicht maskierbarer Interrupt", "Breakpoint",
    "Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
    "Stack-Segment Fault", "General Protection Fault", "Page Fault", "reserviert",
    "x87 Floating-Point Exception", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
    "Virtualization Exception", "Control Protection Exception", "reserviert", "reserviert",
    "reserviert", "reserviert", "reserviert", "reserviert",
    "Hypervisor Injection Exception", "VMM Communication Exception", "Security Exception", "reserviert"
};

/* gibt eine vorzeichenlose zahl als dezimalzahl auf dem terminal aus */
static void print_uint(uint32_t n) {
    char buf[10];
    int i = 0;

    if (n == 0) {
        terminal_putchar('0');
        return;
    }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0)
        terminal_putchar(buf[--i]); // ziffern waren rueckwaerts im puffer, also umgekehrt ausgeben
}

/* zentraler c-handler fuer alle 32 cpu-exceptions, wird aus isr_stubs.s aufgerufen */
void isr_handler(struct registers regs) {
    terminal_writestring("Exception ");
    print_uint(regs.int_no);
    terminal_writestring(": ");
    terminal_writestring(exception_messages[regs.int_no]);
    terminal_writestring("\n");

    // kernel bleibt hier sicher stehen statt mit kaputtem zustand weiterzulaufen
    // hier spaeter dann recovery logik loesung finden
    for (;;)
        asm volatile("hlt");
}
