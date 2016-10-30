#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <types.h>

#define BIT(x) (0x1UL << x)

u8 bitmap_get(void *map, u32 bit_max, u32 bit_index);
u8 bitmap_set(void *map, u32 bit_max, u32 bit_index, u8 bit);

s32 bitmap_ffz(void *map, u32 bit_max);
#endif /* __BITMAP_H__ */
