#include "irq.h"
#include "pic.h"

/* fuer jeden der 16 hardware-irqs ein optionaler c-handler (0 = keiner eingetragen) */
static irq_handler_t irq_handlers[16];

void irq_install(void) {
    pic_remap();
}

void irq_install_handler(uint8_t irq, irq_handler_t handler) {
    if (irq >= 16)
        return;
    irq_handlers[irq] = handler;
    pic_unmask_irq(irq);
}

void irq_uninstall_handler(uint8_t irq) {
    if (irq >= 16)
        return;
    pic_mask_irq(irq);
    irq_handlers[irq] = 0;
}

/* zentraler c-handler fuer alle 16 hardware-irqs, wird aus src/irq_stubs.s aufgerufen */
void irq_handler(struct registers regs) {
    uint8_t irq = regs.int_no - PIC_IRQ_BASE;

    if (pic_is_spurious(irq))
        return; // falscher alarm, nichts zu bearbeiten und nichts zu quittieren

    if (irq_handlers[irq])
        irq_handlers[irq](&regs);

    // ohne eoi liefert der pic keinen weiteren irq mehr aus
    pic_send_eoi(irq);
}
