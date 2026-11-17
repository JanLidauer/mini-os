#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* zaehlt jeden timer-irq (irq 0) mit. der pit laeuft standardmaessig mit ca. 18,2 hz */
extern volatile uint32_t timer_ticks;

void timer_install(void); // in src/timer.c

#endif
