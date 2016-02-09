.text

.global __get_pc
.type __get_pc, %function
__get_pc:
    mov r0, pc
    bx lr

.global __set_pc
.type __set_pc, %function
__set_pc:
    mov pc, r0
    bx lr

.global __get_lr
.type __get_lr, %function
__get_lr:
    mov r0, lr
    bx lr

.global __get_cpsr
.type __get_cpsr, %function
__get_cpsr:
    mrs r0, cpsr
    bx lr

.global __set_cpsr
.type __set_cpsr, %function
__set_cpsr:
    msr cpsr, r0 
    bx lr

.global __get_sp
.type __get_sp, %function
__get_sp:
    mov r0, sp
    bx lr

.global __set_sp
.type __set_sp, %function
__set_sp:
    mov sp, r0  /* note: once you re-set the sp, you lost the call-stack FOREVER */
    bx lr

/*
void __switch_context(old, new);
the register context:

    cpsr
    lr
    sp
    r12
    ..
    r0

*/
.global __switch_context
.type __switch_context, %function
__switch_context:

    /* save old task register context */
    
    mrs r2, cpsr
    mov r3, r13
    push {r2}   /* cpsr */
    push {r14}  /* lr */
    push {r3}   /* r13 */
    push {r12}  /* r12 */
    push {r11}  /* r11 */
    push {r10}  /* r10 */
    push {r9}  /* r9 */
    push {r8}  /* r8 */
    push {r7}  /* r7 */
    push {r6}  /* r6 */
    push {r5}  /* r5 */
    push {r4}  /* r4 */
    push {r3}  /* r3 */ /* it's ok */
    push {r2}  /* r2 */ /* it's ok */
    push {r1}  /* r1 */
    push {r0}  /* r0 */

    str r13, [r0]   /* store to tsk->sp */

    /* restore the new context */

    ldr r2, [r1]
    mov r0, r2      /* get the sp */

    ldr r1, [r0, #0x3c]
    msr cpsr, r1        /* cpsr */

    ldr r1, [r0, #0x38]
    mov lr, r1          /* pc */

    ldr r13, [r0, #0x34] /* r13 */
    ldr r12, [r0, #0x30] /* r12 */
    ldr r11, [r0, #0x2c] /* r11 */
    ldr r10, [r0, #0x28] /* r10 */
    ldr r9, [r0, #0x24] /* r9 */
    ldr r8, [r0, #0x20] /* r8 */
    ldr r7, [r0, #0x1c] /* r7 */
    ldr r6, [r0, #0x18] /* r6 */
    ldr r5, [r0, #0x14] /* r5 */
    ldr r4, [r0, #0x10] /* r4 */
    ldr r3, [r0, #0xc] /* r3 */
    ldr r2, [r0, #0x8] /* r2 */
    ldr r1, [r0, #0x4] /* r1 */
    
    ldr r1, [r0]        /* parameter */
    mov r0, r1
    bx lr



/* __start_first_task(first_task) */
.global __start_first_task
.type __start_first_task, %function
 __start_first_task:
    ldr r2, [r0]
    mov r0, r2      /* get the sp */

    ldr r1, [r0, #0x3c]
    msr cpsr, r1        /* cpsr */

    ldr r1, [r0, #0x38]
    mov lr, r1          /* pc */

    ldr r13, [r0, #0x34] /* r13 */
    ldr r12, [r0, #0x30] /* r12 */
    ldr r11, [r0, #0x2c] /* r11 */
    ldr r10, [r0, #0x28] /* r10 */
    ldr r9, [r0, #0x24] /* r9 */
    ldr r8, [r0, #0x20] /* r8 */
    ldr r7, [r0, #0x1c] /* r7 */
    ldr r6, [r0, #0x18] /* r6 */
    ldr r5, [r0, #0x14] /* r5 */
    ldr r4, [r0, #0x10] /* r4 */
    ldr r3, [r0, #0xc] /* r3 */
    ldr r2, [r0, #0x8] /* r2 */
    ldr r1, [r0, #0x4] /* r1 */
    
    ldr r1, [r0]        /* parameter */
    mov r0, r1
    bx lr

/* just modify the pc */
.global __switch_to
.type __switch_to, %function
__switch_to:
    mov lr, r0
    bx lr


/* die */
.global __die
.type __die, %function
__die:
    svc #0x3

.data

tmp1:
    .word 0x0

tmp2:
    .word 0x0


.end
