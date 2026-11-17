/* Assembler-Stubs fuer die 16 Hardware-IRQs (IRQ 0-15 -> Vektor 32-47).
   Aufbau wie in isr_stubs.s: die CPU springt direkt zum Stub, der bringt den
   Stack in die einheitliche Form (err_code, int_no) und springt zum
   gemeinsamen irq_common_stub. Hardware-IRQs haben keinen Error-Code, also
   legen wir immer eine 0 ab. */

.section .text

.macro IRQ num, vector
.global irq\num
irq\num:
	push $0
	push $\vector
	jmp irq_common_stub
.endm

IRQ 0,  32   /* Timer (PIT) */
IRQ 1,  33   /* Tastatur */
IRQ 2,  34   /* Kaskade zum Slave-PIC (kommt nie selbst an) */
IRQ 3,  35   /* COM2 */
IRQ 4,  36   /* COM1 */
IRQ 5,  37   /* LPT2 / Soundkarte */
IRQ 6,  38   /* Diskettenlaufwerk */
IRQ 7,  39   /* LPT1 / spurious */
IRQ 8,  40   /* Echtzeituhr (RTC) */
IRQ 9,  41   /* frei */
IRQ 10, 42   /* frei */
IRQ 11, 43   /* frei */
IRQ 12, 44   /* PS/2-Maus */
IRQ 13, 45   /* FPU / Coprozessor */
IRQ 14, 46   /* primaerer ATA-Kanal */
IRQ 15, 47   /* sekundaerer ATA-Kanal / spurious */

/* gemeinsamer Teil fuer alle 16 Stubs: rettet alle Register, ruft den
   C-Handler irq_handler auf und stellt danach alles wieder her */
irq_common_stub:
	pusha                  /* rettet eax, ecx, edx, ebx, esp, ebp, esi, edi */

	mov %ds, %ax
	push %eax               /* rettet das aktuelle Datensegment */

	mov $0x10, %ax          /* kernel-datensegment laden (eintrag 2 aus der gdt) */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	cld                     /* System V ABI verlangt geloeschtes Direction-Flag beim C-Aufruf */
	call irq_handler        /* struct registers liegt bereits passend auf dem stack */

	pop %eax                /* altes datensegment zurueckholen */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa                    /* general-purpose register wiederherstellen */
	add $8, %esp            /* int_no und err_code wieder vom stack entfernen */
	iret                    /* zurueck zum unterbrochenen code, stellt auch IF wieder her */
