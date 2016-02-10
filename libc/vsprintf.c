#include <stdarg.h>
#include <types.h>

u32 buf_putc(char *buf, u32 size, u32 *offset, u8 c)
{
    u32 off = *offset;
    if (off < size) {
        buf[off] = c;
        *offset = off + 1;
    }
    return 0;
}

u32 buf_puts(char *buf, u32 size, u32 *offset, char *s)
{
    u32 i;
    for(i=0;s[i]!='\0';i++) {
        buf_putc(buf, size, offset, s[i]);
    }
    return 0;
}

/* buf size at least >= 10, radix = 10, 16 */
char * itoa(char *buf, u32 x, u32 radix)
{
    s32 i;
    memset(buf, 0, 10);

    for(i=9;i>=0;i--) {
        switch (radix) {
            case (10):
                buf[i] =(x % radix) + '0';
                x = x / radix ;
                break;
            case (16):
                if ((x % radix) < 10) {
                    buf[i] = (x % radix) + '0';
                } else {
                    buf[i] = (x % radix) - 10 + 'A';
                }
                x = x / radix ;
                break;
            default:
                return NULL;
        }
    }
    for(i=0;i<9;i++) {
        if (buf[i] != '0') {
            break;
        }
    }


    return &buf[i];

}

/* as simple as possible, only support %c %d %x %s, and don't care the negative num */
/* of course, I don't care the efficiency as well */
int vsnprintf(char *buf, u32 size, const char *fmt, va_list args)
{
    u32 i, offset, len;
    u8  c;
    u32 d, x;
    char *s, *b;
    va_list ap;

    char num[11]; /* 2^32 = 4294967296 */

    offset = 0;
    memset(buf, 0, size);
    memset(num, 0, sizeof(num));
    len = strlen(fmt);

    for(i=0;i<len;i++) {
        if (fmt[i] == '%') {
            if ((i+1) == len) { /* % is the last char of the string */
                buf_putc(buf, size, &offset, fmt[i]);
                break;
            } else {
                switch (fmt[i+1]) {
                    case ('c'):
                        c = va_arg(args, u32);
                        buf_putc(buf, size, &offset, c);
                        i++;
                        break;
                    case ('d'):
                        d = va_arg(args, u32);
                        b = itoa(num, d, 10);
                        buf_puts(buf, size, &offset, b);
                        i++;
                        break;
                    case ('x'):
                    case ('X'):
                        x = va_arg(args, u32);
                        b = itoa(num, x, 16);
                        buf_puts(buf, size, &offset, b);
                        i++;
                        break;
                    case ('s'):
                        s = va_arg(args, char *);
                        b = s;
                        if (b == NULL) {
                            buf_puts(buf, size, &offset, "(null)");
                        } else {
                            buf_puts(buf, size, &offset, b);
                        }
                        i++;
                        break;
                    default:
                        buf_putc(buf, size, &offset, fmt[i]);   /* the '%' */
                        break;
                }
            }
        } else {
            /*  ordinary character */
            buf_putc(buf, size, &offset, fmt[i]);
        }
    }

    buf[size-1] = '\0';
}

