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

.end
