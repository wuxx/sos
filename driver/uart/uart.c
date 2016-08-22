#include <libc.h>
#include <memory_map.h>

#include "mmio.h"
#include "uart.h"
#include "shell.h"
#include "gpio.h"
#include "timer.h"

#define UART0       /* pl011 uart */
/*#define UART1*/   /* mini uart, need debug. */

#define UART_IO_SIZE 256

PRIVATE u32  uart_recv_buf_index = 0;
PRIVATE char uart_recv_buf[UART_IO_SIZE] = {0};

PUBLIC void uart_wait_fifo_empty()
{
    while(1) {
        if ((readl(UART0_FR) & (1 << 7))) {
            break;
        }
    }
}

PUBLIC void uart_putc(u8 byte) {
    // wait for UART to become ready to transmit
    while (1) {
        if (!(readl(UART0_FR) & (1 << 5))) {
            break;
        }
    }
    writel(UART0_DR, byte);
    uart_wait_fifo_empty();
}

PUBLIC void uart_puts(const char *str) {
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

s8 uart_recv()
{

    if ((readl(UART0_FR) & (1 << 4)) == 0) {    /* if RX FIFO not empty */
        return readl(UART0_DR);
    } else {
        return 0;
    }
}

PRIVATE s32 uart_irq_handler(u32 irq_nr)
{
    u8 ch;
    u32 raw_status;

    /* handle one character */
    while((ch = uart_recv())) {
        /* uart_printf("recv [%x][%c] \n", ch, ch); */
        if (ch == '\n') {   /* sscom will send '\r\n' we ignore the '\n' */
            continue;
        }
        if (uart_recv_buf_index == (UART_IO_SIZE - 1) && ch != '\r') {
            uart_puts("cmd too long!\n");
            uart_recv_buf_index = 0;
            return EINVAL;

        }

        if (ch == '\r') {
            uart_recv_buf[uart_recv_buf_index] = '\0';  /* terminate the string. */
            shell(uart_recv_buf);

            uart_recv_buf_index = 0;
            uart_puts("\nsos>");
            break;
        } else {
            uart_recv_buf[uart_recv_buf_index] = ch;
            uart_recv_buf_index++;
        }


        /* echo */
        if (ch == '\r') {
            uart_putc('\r');
            uart_putc('\n');
        } else {
            uart_putc(ch);
        }
    }

    raw_status = readl(UART0_RIS);
    if ((raw_status >> 7) & 0xf) {
        uart_printf("uart error %x \n",  raw_status);
        writel(UART0_ICR, 0xFFFFFFFF);
    }

    writel(UART0_ICR, 1 << 4);

    return 0;
}

PUBLIC s32 uart_printf(const char *format, ...)
{
    u32 len;
    va_list args;
    static char format_buf[UART_IO_SIZE] = {0};

    va_start(args, format);
    len = vsnprintf(format_buf, sizeof(format_buf), format, args);
    va_end(args);

    uart_puts(format_buf);

    return len; 
}

PUBLIC void uart_init() 
{

    // Setup the GPIO pin 14 && 15.
    set_gpio_function(14, ALT_FUNC_0);
    set_gpio_function(15, ALT_FUNC_0);

    // Disable PL011_UART.
    writel(UART0_CR, 0x00000000);

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    writel(GPPUD, 0x00000000);
    clk_delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    writel(GPPUDCLK0, (1 << 14) | (1 << 15));
    clk_delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    writel(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    writel(UART0_ICR, 0x7FF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.

    /* uart clk: 3000000 */
    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    writel(UART0_IBRD, 1);
    writel(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    writel(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
    // Disable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    /* writel(UART0_LCRH, (0 << 4) | (1 << 5) | (1 << 6)); */

    // Mask all interrupts.
#if 0
    writel(UART0_IMSC, 0xFFFFFFFF); 
#else
    writel(UART0_IMSC, (1 << 1) | (1 << 4) |
            (1 << 6) | (1 << 7) | (1 << 8) |
            (1 << 9) | (1 << 10));
#endif

    // Enable UART, receive & transfer part of UART.
    writel(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));

    request_irq(IRQ_UART, uart_irq_handler);
    enable_irq(IRQ_UART);
}
