#ifndef __STRING_H__
#define __STRING_H__
#include <types.h>
#include <stdarg.h>

u32 strlen(const char *str);
s32 strcmp(char *s1, char *s2);
u32 atoi(char *str);
char * itoa(char *buf, u32 x, u32 radix);
void * memset(void *s, s32 c, u32 size);
void * memcpy(void *dst, void *src, u32 size);
s32 memcmp(void *s1, void *s2, u32 n);
u32 ffs(u32 x);
u32 fls(u32 x);
#endif /* __STRING_H__ */

