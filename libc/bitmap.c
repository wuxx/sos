#include <libc.h>

u8 bitmap_get(void *map, u32 bit_max, u32 bit_index)
{
    u8  bit;
    u32 word_index, word_offset;
    u32 *pmap;

    bit_index = bit_index % bit_max;

    pmap = map;
    word_index  = bit_index / 32; 
    word_offset = bit_index % 32; 

    bit = pmap[word_index] & (0x1 << word_offset);

    return bit;
}

u8 bitmap_set(void *map, u32 bit_max, u32 bit_index, u8 bit)
{
    u32 word_index, word_offset;
    u32 *pmap;
    u32 bit_mask;

    bit_index = bit_index % bit_max;

    pmap = map;
    word_index  = bit_index / 32; 
    word_offset = bit_index % 32; 

    if (bit == 0) {
        bit_mask = ~(0x1 << word_offset);
        pmap[word_index] = (pmap[word_index]) & bit_mask;
    } else {    /* b == 1 */
        bit_mask = (0x1 << word_offset);
        pmap[word_index] = (pmap[word_index]) | bit_mask;
    }   

    return 0;
}

/* find first zero */
s32 bitmap_ffz(void *map, u32 bit_max)
{
    u32 i;
    for(i = 0; i < bit_max; i++) {
        if (bitmap_get(map, bit_max, i) == 0) {
            return i;
        }
    }
    return -1;
}
