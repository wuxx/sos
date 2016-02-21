#ifndef __CPU_H__
#define __CPU_H__
#include <types.h>

/*
cpu_context in irq mode sp
            lr
            r12
            r11
            r10
            r9
            r8
            r7
            r6
            r5
            r4
            r3
            r2
            r1
            r0
            cpsr 
sp_irq ->   r13 (sp_user or sp_system)
 */
enum CPU_MODE_E {
    MODE_USER   = 16,
    MODE_FIQ    = 17,
    MODE_IRQ    = 18,
    MODE_SVC    = 19,
    MODE_SECMT  = 22,
    MODE_ABORT  = 23,
    MODE_UNDEF  = 27,
    MODE_SYSTEM = 31,
};

struct cpu_context {
    u32 cpsr;   /* as spsr_xxx in irq mode */
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;
    u32 r12;
    u32 sp;
    u32 lr;
    u32 pc;
};

#if 0
static __inline__ u32 __get_lr()
{
    register u32 __r0 __asm("r0");

    __asm__ volatile (
            "mov r0, lr\n\t"
            :   "=r" (__r0)
            :
            :
        );
    return __r0;
}
#endif

extern u32 __get_pc();
extern u32 __set_pc();

extern u32 __get_lr();

extern u32 __get_sp();
extern u32 __set_sp();

extern u32 __get_cpsr();
extern u32 __set_cpsr();

#endif /* __CPU_H__ */
