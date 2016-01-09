#include "vsprintf.h"

static char buf[1024];

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    vsnprintf(buf,sizeof(buf), fmt, args);
    va_end(args);
    /*con_write(buf);*/
}
