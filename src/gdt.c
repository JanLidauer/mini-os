#include "gdt.h"

struct gdt_ptr var1;
struct gdt_entry arr1[3];

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    //base wird mit vollen 32 bit an das gdt entry array übergeben, jetzt müssen die 32 bit wie das struct beschreibt in teile low middle und high aufgeteilt werden mittels bit shift
    arr1[num].base_low = base & 0xFFFF; //ersten 16bit
    arr1[num].base_middle = (base >> 16) & 0xFF; //nächste 8 bit
    arr1[num].base_high = (base >> 24) & 0xFF;  // letzte 8 bit
    arr1[num].limit_low = limit & 0xFFFF;
    arr1[num].granularity = (limit >> 16) & 0x0F; //obere 4 bit vom limit (bit 16-19) in die unteren 4 bit
    arr1[num].granularity |= gran & 0xF0; //flags aus gran in die oberen 4 bit
    arr1[num].access = access; //1:1 übernehmen
}

void gdt_install() {
    var1.limit = sizeof(struct gdt_entry) * 3 - 1; //größe der gdt in bytes minus 1
    var1.base  = (uint32_t)&arr1; //adresse von arr1

    gdt_set_gate(0, 0, 0, 0, 0);                //null deskriptor
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); //code segment: base 0, ganzer adressraum, ring 0, ausführbar
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); //daten segment: base 0, ganzer adressraum, ring 0, lesbar/schreibbar

    gdt_flush((uint32_t)&var1); //lgdt + segmentregister neu laden (assembler)
}
