

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

ResetHandler:
    mov sp,#0x8000
    b main
    b . 

UndHandler:
    bl General_Exc_Handler

SwiHandler:
    bl General_Exc_Handler

PrefetchHandler:
    bl General_Exc_Handler

AbortHandler:
    bl General_Exc_Handler

IrqHandler:
    bl General_Irq_Handler

FiqHandler:
    bl General_Exc_Handler

