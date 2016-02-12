#include <memory_map.h>
#include <libc.h>

#include "log.h"

static char buf[1024] = {0};

s32 test_libc_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i;
    i = atoi(argv[2]);
    switch(i) {
        case (0): /* itoa test */
            itoa(buf, 0x12345678, 10);
            uart_puts(buf);

            uart_puts("\n");
            itoa(buf, 0x12345678, 16);
            uart_puts(buf);

            uart_puts("\n");
            itoa(buf, 0xABCDEF12, 16);
            uart_puts(buf);
            break;
        case (1):   /* vsnprintf test */
            PRINT_EMG("hello, world!\n");
            PRINT_EMG("%%%%%%%%%%%%%%%");
            PRINT_EMG("hello %c \n", 'A');
            PRINT_EMG("hello %d \n", 12345678);
            PRINT_EMG("hello %x \n", 0x12345678);
            PRINT_EMG("hello %s \n", "world!");
            PRINT_EMG("hello %X \n", 0x12345678);
            break;
        case (2):   /* assert test */
            assert(1==2);
            break;
        default:
            return -1;
    }

    return ret;
}
