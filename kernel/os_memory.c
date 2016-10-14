#include <system_config.h>
#include <libc.h>
#include <os.h>
#include "log.h"

/* |mp|first...|last| */
struct __memory_pool__ * memory_pool_init(void *mem, u32 size)
{
    struct __memory_pool__ *mp;
    struct pnode *first, *last;

    if (size <= sizeof(struct __memory_pool__) + 2 * sizeof(struct pnode)) {
        return NULL;
    }

    size = size - sizeof(struct __memory_pool__) - sizeof(struct pnode);

    mp = (struct __memory_pool__ *)mem;
    first = (struct pnode *)((u32)mem + sizeof(struct __memory_pool__));
    last  = (struct pnode *)((u32)mem + size - sizeof(struct __memory_pool__));

    last->next = NULL;
    last->prev = NULL;
    last->size = 0;

    first->next = last;
    first->prev = (struct pnode *)mp;
    first->size = size;

    mp->next = first;
    mp->size = size;

    return mp;
}

void * memory_pool_alloc(struct __memory_pool__ *mp, u32 size)
{
    u32 left_size = 0;

    struct pnode *ppn, *ppnn;

    ppn = mp->next;

    while(ppn->size != 0) {

        if (ppn->size > 0 && ppn->size >= (sizeof(struct pnode) + size)) {
            left_size = (u32)(ppn->next) - (u32)ppn - sizeof(struct pnode) - size;
            if (left_size > sizeof(struct pnode))  {
                ppnn = (struct pnode *)((u32)ppn + sizeof(struct pnode) + size);

                ppnn->size = ppn->size - (sizeof(struct pnode) + size);

                ppnn->next = ppn->next;
                ppnn->prev = ppn;

                ppn->next = ppnn;

                ppn->size = ppn->size - ppnn->size;

            }

            break;
        }

        ppn = ppn->next;
    }

    if (ppn->size == 0) {   /* the last node */
        return NULL;
    } else {
        ppn->size = -ppn->size;
        return (void *)((u32)(ppn) + sizeof(struct pnode));
    }

}

void memory_pool_free(struct __memory_pool__ *mp, void *mem)
{
    struct pnode *ppn;
    ppn = (struct pnode *)((u32)mem - sizeof(struct pnode));
    kassert(ppn->size <= 0 && ppn->next != NULL && ppn->prev != NULL);

    ppn->size = -ppn->size;
    /* back merge */
    if (ppn->next->size > 0) {
        ppn->size += ppn->next->size;
        ppn->next->next->prev = ppn;
        ppn->next = ppn->next->next;
    }

    /* front merge */
    if (ppn != mp->next) {
        if (ppn->prev->size > 0)  {
            ppn->prev->size += ppn->size;
            ppn->prev->next  = ppn->next;
            ppn->next->prev  = ppn->prev;
        }
    }

}

struct __memory_box__ * memory_box_init(void *mem, u32 size, u32 cell_size)
{
    u32 cell_nr1, cell_nr2;
    struct __memory_box__ *mb;

    if (size >= sizeof(struct __memory_box__)) {
        return NULL;
    }

    memset(mem, 0, size);

    mb = (struct __memory_box__ *)mem;

    size = size - sizeof(struct __memory_box__);
    cell_nr2  = size / cell_size; 
    cell_nr1  = (size % cell_size) * sizeof(u8);

    while (cell_nr1 < cell_nr2) {
        cell_nr2--;
        cell_nr1 = cell_nr1 + cell_size * sizeof(u8);
    }


    mb->addr      = (void *)((u32)mem + size - cell_size * cell_nr2);
    mb->cell_nr   = cell_nr2;
    mb->cell_size = cell_size;

    return mb;
}

void * memory_box_alloc(struct __memory_box__ *mb)
{
    s32 i;
    if ((i = bitmap_ffz(mb->map, mb->cell_nr)) == -1) {
        return NULL;
    }

    bitmap_set(mb->map, mb->cell_nr, i, 1);
    return (void *)((u32)(mb->addr) + i * mb->cell_size);
}

void memory_box_free(struct __memory_box__ *mb, void *cell)
{
    u32 i;
    i = ((u32)(cell) - (u32)(mb->addr)) / mb->cell_size;

    kassert(i < mb->cell_nr);

    bitmap_set(mb->map, mb->cell_nr, i, 0);
}
