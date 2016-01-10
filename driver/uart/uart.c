#include <libc.h>
#include <memory_map.h>

#include "mmio.h"
#include "uart.h"

char buf[1024] = {0};
 
/*
 * Initialize UART0.
 */
void uart_init() {
    // Disable UART0.
    writel(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.
 
    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    writel(GPPUD, 0x00000000);
    delay(150);
 
    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    writel(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
 
    // Write 0 to GPPUDCLK0 to make it take effect.
    writel(GPPUDCLK0, 0x00000000);
 
    // Clear pending interrupts.
    writel(UART0_ICR, 0x7FF);
 
    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.
 
    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    writel(UART0_IBRD, 1);
    writel(UART0_FBRD, 40);
 
    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    writel(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
    // Mask all interrupts.
    writel(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) |
            (1 << 6) | (1 << 7) | (1 << 8) |
            (1 << 9) | (1 << 10));
 
    // Enable UART0, receive & transfer part of UART.
    writel(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
/*
 * Transmit a byte via UART0.
 * u8 Byte: byte to send.
 */
void uart_putc(u8 byte) {
    // wait for UART to become ready to transmit
    while (1) {
        if (!(readl(UART0_FR) & (1 << 5))) {
        break;
    }
    }
    writel(UART0_DR, byte);
}
 
/*
 * print a string to the UART one character at a time
 * const char *str: 0-terminated string
 */
void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
            uart_putc('\n');
            str++;
        }
        else {
            uart_putc(*str++);
        }
    }
}

void uart_printf(const char* fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vsnprintf(buf,sizeof(buf), fmt, args);
    va_end(args);
    uart_puts(buf);
}
