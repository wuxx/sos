#ifndef __UART_H__
#define __UART_H__
#include <libc.h>

void uart_putc(u8 byte);
void uart_puts(const char *str);
void uart_init();
void uart_wait_fifo_empty();
s32 uart_printf(const char *format, ...);

#endif /* __UART_H__ */
