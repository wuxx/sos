#ifndef __COMMON_H__
#define __COMMON_H__

#define ALLOC_ALIGN_BUFFER(type, name, size, align)         \
        char __##name[ROUND(size * sizeof(type), align) + (align - 1)]; \
                                    \
    type *name = (type *) ALIGN((u32)__##name, align)
#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size)          \
        ALLOC_ALIGN_BUFFER(type, name, size, ARCH_DMA_MINALIGN)

#define ROUND(a,b)      (((a) + (b) - 1) & ~((b) - 1))
#define ALIGN(x,a)      __ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#endif /* __COMMON_H__ */
