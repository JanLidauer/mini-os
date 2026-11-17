#include "pic.h"
#include "io.h"

#define PIC1_CMD   0x20   // master: irq 0-7
#define PIC1_DATA  0x21
#define PIC2_CMD   0xA0   // slave: irq 8-15
#define PIC2_DATA  0xA1

#define PIC_EOI        0x20
#define PIC_READ_ISR   0x0B  // naechster lesezugriff auf den cmd-port liefert das in-service-register

#define ICW1_INIT  0x10
#define ICW1_ICW4  0x01
#define ICW4_8086  0x01

/* der pic bildet die irqs standardmaessig auf vektor 8-15 ab, das kollidiert mit
   den cpu-exceptions (8 = double fault, 13 = general protection fault, ...).
   deshalb verschieben wir sie auf 32-47. danach sind alle irqs gesperrt, jeder
   treiber gibt seine leitung selbst mit pic_unmask_irq frei. */
void pic_remap(void) {
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4); io_wait(); // init-sequenz starten
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4); io_wait();

    outb(PIC1_DATA, PIC_IRQ_BASE);     io_wait();     // master: irq 0-7  -> vektor 32-39
    outb(PIC2_DATA, PIC_IRQ_BASE + 8); io_wait();     // slave:  irq 8-15 -> vektor 40-47

    outb(PIC1_DATA, 4); io_wait();                    // master: slave haengt an irq 2 (bitmaske)
    outb(PIC2_DATA, 2); io_wait();                    // slave: seine kaskaden-id ist 2

    outb(PIC1_DATA, ICW4_8086); io_wait();            // 8086-modus
    outb(PIC2_DATA, ICW4_8086); io_wait();

    outb(PIC1_DATA, 0xFB);                            // master: alles gesperrt ausser irq 2 (kaskade zum slave)
    outb(PIC2_DATA, 0xFF);                            // slave: alles gesperrt
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_CMD, PIC_EOI); // irq kam vom slave: auch dem slave bescheid geben
    outb(PIC1_CMD, PIC_EOI);     // der master braucht immer ein eoi (auch fuer slave-irqs, wegen der kaskade)
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t bit = (irq < 8) ? irq : irq - 8;
    outb(port, inb(port) & ~(1 << bit));
}

void pic_mask_irq(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t bit = (irq < 8) ? irq : irq - 8;
    outb(port, inb(port) | (1 << bit));
}

/* irq 7 und irq 15 koennen "spurious" (falsch) auftreten, ohne dass wirklich ein
   geraet etwas gemeldet hat. nur im in-service-register steht, ob er echt ist.
   ein falscher irq 7 bekommt kein eoi, bei einem falschen irq 15 braucht nur der
   master eins (er hat die kaskade-leitung gesehen, der slave nicht) */
uint8_t pic_is_spurious(uint8_t irq) {
    if (irq != 7 && irq != 15)
        return 0;

    uint16_t cmd_port = (irq == 7) ? PIC1_CMD : PIC2_CMD;
    outb(cmd_port, PIC_READ_ISR);
    if (inb(cmd_port) & 0x80)
        return 0; // bit 7 gesetzt: der irq ist echt

    if (irq == 15)
        outb(PIC1_CMD, PIC_EOI);
    return 1;
}
