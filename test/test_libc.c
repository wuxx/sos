#include <memory_map.h>
#include <libc.h>

static char buf[1024] = {0};

extern int vsnprintf(char *buf, u32 size, const char *fmt, va_list args);
void test_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    dump_mem((u32)args, 10);
    vsnprintf(buf,sizeof(buf), fmt, args);
    va_end(args);
    uart_puts(buf);
}

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
            test_printf("hello, world!\n");
            test_printf("%%%%%%%%%%%%%%%");
            test_printf("hello %c \n", 'A');
            test_printf("hello %d \n", 12345678);
            test_printf("hello %x \n", 0x12345678);
            test_printf("hello %s \n", "world!");
            test_printf("hello %X \n", 0x12345678);
            break;
        case (2):   /* assert test */
            assert(1==2);
            break;
        default:
            return -1;
    }

    return ret;
}
