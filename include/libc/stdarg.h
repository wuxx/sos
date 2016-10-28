#ifndef __STDARG_H__
#define __STDARG_H__
#include <types.h>

typedef char* va_list;

#define __va_size(type) \
       (((sizeof(type) + sizeof(long) - 1) / sizeof (long)) * sizeof (long))

#define va_start(ap, last) \
       ((ap) = ((char*)&last) + __va_size(last))

#define va_arg(ap, type) \
       (*(type*)((ap)+= __va_size(type), (ap) - __va_size(type)))

#define va_end(va_list) ((void)0)

int vsnprintf(char *buf, u32 size, const char *fmt, va_list args);
#endif /* __STDARG_H__ */
