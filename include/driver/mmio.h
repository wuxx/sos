#ifndef __MMIO_H__
#define __MMIO_H__

#include <types.h>

#define readb(addr)  (*((u8*)addr))
#define writeb(data, addr) (*((u8*)addr) = data)

#define get_bit(x, bit_index) ((x >> bit_index) & 0x1)

static inline void set_bit(u32 *x, u32 bit_index, u32 b) {
    u32 _x;
    u32 bit_mask;
    _x = *x;
    if (get_bit(_x, bit_index) != b) {
        if (b == 0) {
            bit_mask = ~(0x1 << bit_index);
            *x = (_x) & bit_mask;
        } else {    /* b == 1 */
            bit_mask = (0x1 << bit_index);
            *x = (_x) | bit_mask;
        }
    }
}

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
