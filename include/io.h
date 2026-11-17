#ifndef IO_H
#define IO_H

#include <stdint.h>

/* schreibt ein byte an einen hardware-port (separater io-adressraum, nicht
   normaler speicher). wird z.b. fuer pic und tastatur gebraucht */
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* liest ein byte von einem hardware-port */
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* kurze pause fuer alte hardware, die zwischen zwei port-zugriffen zeit braucht.
   port 0x80 ist ungenutzt, der schreibzugriff dauert nur ein paar mikrosekunden */
static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif
