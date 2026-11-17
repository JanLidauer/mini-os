#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/* erster interrupt-vektor fuer hardware-irqs: irq 0-15 -> vektor 32-47 */
#define PIC_IRQ_BASE 32

void pic_remap(void);                // in src/pic.c
void pic_send_eoi(uint8_t irq);      // "interrupt fertig bearbeitet" an den pic melden
void pic_unmask_irq(uint8_t irq);    // irq-leitung freigeben
void pic_mask_irq(uint8_t irq);      // irq-leitung sperren
uint8_t pic_is_spurious(uint8_t irq); // irq 7/15: war der irq falsch (spurious)? dann schon quittiert, kein eoi mehr noetig

#endif
