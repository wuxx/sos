#ifndef __INT_H__
#define __INT_H__
#include <types.h>
s32 request_irq(u32 irq_nr, func_1 irq_handler);
s32 release_irq(u32 irq_nr);
s32 enable_irq(u32 irq_nr);
s32 disable_irq(u32 irq_nr);
s32 lockup(char *file_name, char *func_name, u32 line_num, char *desc);
s32 int_init();

#endif /* __INT_H__ */
