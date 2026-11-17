#include "timer.h"
#include "irq.h"

/* volatile: wird im irq-handler veraendert, der compiler darf den wert in kernel_main nicht cachen */
volatile uint32_t timer_ticks = 0;

static void timer_callback(struct registers *regs) {
    (void)regs;
    timer_ticks++;
}

void timer_install(void) {
    irq_install_handler(0, timer_callback); // traegt den handler ein und gibt irq 0 am pic frei
}
