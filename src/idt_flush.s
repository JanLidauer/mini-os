/* void idt_flush(uint32_t idt_ptr_address)
   laedt die idt mit lidt, genau wie gdt_flush.s */

.section .text
.global idt_flush
.type idt_flush, @function
idt_flush:
	mov 4(%esp), %eax   /* erstes argument (adresse vom idt_ptr) vom stack holen */
	lidt (%eax)         /* idtr register mit limit+base laden */
	ret

.size idt_flush, . - idt_flush
