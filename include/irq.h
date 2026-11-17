#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include "isr.h"

/* c-handler fuer einen hardware-irq (0-15) */
typedef void (*irq_handler_t)(struct registers *regs);

void irq_install(void);                                  // pic remappen (in src/irq.c)
void irq_install_handler(uint8_t irq, irq_handler_t handler);   // handler eintragen + irq freigeben
void irq_uninstall_handler(uint8_t irq);                 // handler entfernen + irq sperren
void irq_handler(struct registers regs);                 // zentraler handler, wird aus src/irq_stubs.s aufgerufen

/* stubs fuer die 16 hardware-irqs (vektor 32-47), siehe src/irq_stubs.s */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif
