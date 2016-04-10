#ifndef __SYSTEST_H__
#include <libc.h>
void dump_mem(u32 addr, u32 word_nr);
s32 systest(u32 argc, char **argv);
#define __SYSTEST_H__
#endif /* __SYSTEST_H__ */
