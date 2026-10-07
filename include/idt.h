#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* ein Eintrag (Gate) in der IDT: zeigt auf die Adresse eines Interrupt-Handlers */
struct idt_entry {
    uint16_t offset_low;   // untere 16 bit der Handler-Adresse
    uint16_t selector;     // Code-Segment-Selector aus der GDT (bei uns 0x08)
    uint8_t  zero;         // muss immer 0 sein
    uint8_t  type_attr;    // Present-Bit, Ring (DPL) und Gate-Typ
    uint16_t offset_high;  // obere 16 bit der Handler-Adresse
} __attribute__((packed));

/* wird dem Prozessor per lidt uebergeben: Adresse + Groesse der IDT */
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_install(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void idt_flush(uint32_t idt_ptr_address); // in src/idt_flush.s

#endif
