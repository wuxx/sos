#ifndef __MMIO_H__
#define __MMIO_H__

#include <types.h>

#define readb(data, addr)  (*((u8*)addr))
#define writeb(data, addr) (*((u8*)addr) = data)

static inline void writel(u32 addr, u32 data) {
    u32 *ptr = (u32*)addr;
    asm volatile("str %[data], [%[addr]]"
            : 
            : [addr]"r"(ptr), [data]"r"(data));
}           

static inline u32 readl(u32 addr) {
    u32 *ptr = (u32*)addr;
    u32 data;
    asm volatile("ldr %[data], [%[addr]]"
            : [data]"=r"(data) 
            : [addr]"r"(ptr));
    return data;
}   

#endif /* __MMIO_H__ */
