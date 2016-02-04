.set    MODE_USR, 0x10
.set    MODE_FIQ, 0x11
.set    MODE_IRQ, 0x12
.set    MODE_SVC, 0x13
.set    MODE_ABT, 0x17
.set    MODE_UND, 0x1B
.set    MODE_SYS, 0x1F

.set    I_BIT, 0x80
.set    F_BIT, 0x40

.text
.code   32
.balign 4

.section .init
.globl _start
_start:
        ldr     pc, _reset
        ldr     pc, _undefined
        ldr     pc, _swi
        ldr     pc, _prefetch
        ldr     pc, _abort
        nop
        ldr     pc, _irq
        ldr     pc, _fiq

_reset:
        .word   ResetHandler
_undefined:
        .word   UndHandler
_swi:
        .word   SwiHandler
_prefetch:
        .word   PrefetchHandler
_abort:
        .word   AbortHandler
        .word   0
_irq:
        .word   IrqHandler
_fiq:
        .word   FiqHandler



UndHandler:
    b ExcHandler

SwiHandler:
    b ExcHandler

PrefetchHandler:
    b ExcHandler

AbortHandler:
    b ExcHandler

FiqHandler:
    b ExcHandler


.global ResetHandler
ResetHandler:
    mov r0,#0x8000
    mov r1,#0x0000
    /* branch instructions 8 words */
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
    /* vector table 8 words */
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

    mov     r0, #0
    ldr     r1, =__bss_start__
    ldr     r2, =__bss_end__
clear_bss:
    cmp     r1, r2
    strlo   r0, [r1], #4
    blo     clear_bss

    ldr     r0, =__ram_end__

    /* Undef */
    msr     CPSR_c, #MODE_UND | I_BIT | F_BIT
    mov     sp, r0
    ldr     r1, =__und_stack_size__
    sub     r0, r0, r1

    /* Abort */
    msr     CPSR_c, #MODE_ABT | I_BIT | F_BIT
    mov     sp, r0
    ldr     r1, =__abt_stack_size__
    sub     r0, r0, r1

    /* FIQ */
    msr     CPSR_c, #MODE_FIQ | I_BIT | F_BIT
    mov     sp, r0
    ldr     r1, =__fiq_stack_size__
    sub     r0, r0, r1

    /* IRQ */
    msr     CPSR_c, #MODE_IRQ | I_BIT | F_BIT
    mov     sp, r0
    ldr     r1, =__irq_stack_size__
    sub     r0, r0, r1

    /* Supervisor */
    msr     CPSR_c, #MODE_SVC | I_BIT | F_BIT
    mov     sp, r0
    ldr     r1, =__svc_stack_size__
    sub     r0, r0, r1

    /* System */
    msr     CPSR_c, #MODE_SYS | I_BIT | F_BIT
    mov     sp, r0
    bl main
    b . 
