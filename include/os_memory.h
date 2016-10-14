#ifndef __OS_MEMORY_H__
#define __OS_MEMORY_H__

#include <libc.h>

struct pnode {
    struct pnode *next;
    struct pnode *prev;
    s32 size;   /* include the struct pnode itself */
};

struct __memory_pool__ {
    struct pnode *next;
    u32 size;
};


struct __memory_box__ {
    void *addr;
    u32 cell_nr;
    u32 cell_size;
    u8  map[0];
};

struct __memory_pool__ * memory_pool_init(void *mem, u32 size);
void * memory_pool_alloc(struct __memory_pool__ *mp, u32 size);
void memory_pool_free(struct __memory_pool__ *mp, void *mem);

struct __memory_box__ * memory_box_init(void *mem, u32 size, u32 cell_size);
void * memory_box_alloc(struct __memory_box__ *mb);
void memory_box_free(struct __memory_box__ *mb, void *cell);

#endif /* __OS_MEMORY_H__ */
