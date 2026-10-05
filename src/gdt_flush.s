/* void gdt_flush(uint32_t gdt_ptr_address)
   laedt die gdt mit lgdt und setzt danach alle segmentregister neu */

.section .text
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
	mov 4(%esp), %eax   /* erstes argument (adresse vom gdt_ptr) vom stack holen */
	lgdt (%eax)         /* gdtr register mit limit+base laden */

	mov $0x10, %ax      /* 0x10 = eintrag 2 (daten segment), 2 * 8 bytes */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	ljmp $0x08, $flush_done /* far jump: setzt cs auf eintrag 1 (code segment, 1 * 8 bytes) */
flush_done:
	ret

.size gdt_flush, . - gdt_flush
