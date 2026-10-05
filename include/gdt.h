#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));  //prevents the compiler from adding additional fill bytes between the attributes which could cause errors

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;

} __attribute__((packed));

void gdt_install(void);
void gdt_flush(uint32_t gdt_ptr_address); // in src/gdt_flush.s

#endif
