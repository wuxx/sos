#include <types.h>

struct cpu_context {
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;
    u32 sp;
    u32 cpsr;

    /* cpu auto-saved */
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r12;
    u32 lr;
    u32 pc;
};

static __inline__ __get_lr()
{
    __asm__ volatile ("mov r0, lr");
}

extern u32 __get_sp();
extern u32 __get_cpsr();