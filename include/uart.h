#ifndef __UART_H__
#define __UART_H__
#include <libc.h>

void uart_puts(const char *str);
void uart_init();

#endif /* __UART_H__ */
