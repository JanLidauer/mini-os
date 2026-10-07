/* Assembler-Stubs fuer die 32 CPU-Exceptions (Index 0-31).
   CPU springt bei einer Exception direkt zum passenden isrN, noch bevor
   irgendwelche Register gerettet wurden. Jeder Stub bringt den Stack in eine
   einheitliche Form und springt dann gemeinsam zu isr_common_stub. */

.section .text

/* Exceptions ohne Error-Code: die CPU legt hier keinen Error-Code ab, wir
   legen stattdessen selbst eine 0 ab, damit der Stack-Aufbau fuer alle
   Exceptions gleich aussieht */
.macro ISR_NOERR num
.global isr\num
isr\num:
	push $0
	push $\num
	jmp isr_common_stub
.endm

/* Exceptions mit Error-Code: die CPU legt den Error-Code hier schon selbst ab */
.macro ISR_ERR num
.global isr\num
isr\num:
	push $\num
	jmp isr_common_stub
.endm

ISR_NOERR 0   /* Division durch Null */
ISR_NOERR 1   /* Debug */
ISR_NOERR 2   /* Nicht maskierbarer Interrupt */
ISR_NOERR 3   /* Breakpoint */
ISR_NOERR 4   /* Overflow */
ISR_NOERR 5   /* Bound Range Exceeded */
ISR_NOERR 6   /* Invalid Opcode */
ISR_NOERR 7   /* Device Not Available */
ISR_ERR   8   /* Double Fault */
ISR_NOERR 9   /* Coprocessor Segment Overrun */
ISR_ERR   10  /* Invalid TSS */
ISR_ERR   11  /* Segment Not Present */
ISR_ERR   12  /* Stack-Segment Fault */
ISR_ERR   13  /* General Protection Fault */
ISR_ERR   14  /* Page Fault */
ISR_NOERR 15  /* reserviert */
ISR_NOERR 16  /* x87 Floating-Point Exception */
ISR_NOERR 17  /* Alignment Check */
ISR_NOERR 18  /* Machine Check */
ISR_NOERR 19  /* SIMD Floating-Point Exception */
ISR_NOERR 20  /* Virtualization Exception */
ISR_NOERR 21  /* Control Protection Exception */
ISR_NOERR 22  /* reserviert */
ISR_NOERR 23  /* reserviert */
ISR_NOERR 24  /* reserviert */
ISR_NOERR 25  /* reserviert */
ISR_NOERR 26  /* reserviert */
ISR_NOERR 27  /* reserviert */
ISR_NOERR 28  /* Hypervisor Injection Exception */
ISR_NOERR 29  /* VMM Communication Exception */
ISR_NOERR 30  /* Security Exception */
ISR_NOERR 31  /* reserviert */

/* gemeinsamer Teil fuer alle 32 Stubs: rettet alle Register, ruft den
   C-Handler isr_handler auf und stellt danach alles wieder her */
isr_common_stub:
	pusha                  /* rettet eax, ecx, edx, ebx, esp, ebp, esi, edi */

	mov %ds, %ax
	push %eax               /* rettet das aktuelle Datensegment */

	mov $0x10, %ax          /* kernel-datensegment laden (eintrag 2 aus der gdt) */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	call isr_handler         /* struct registers liegt bereits passend auf dem stack */

	pop %eax                /* altes datensegment zurueckholen */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa                    /* general-purpose register wiederherstellen */
	add $8, %esp            /* int_no und err_code wieder vom stack entfernen */
	iret                    /* zurueck zum unterbrochenen code */
