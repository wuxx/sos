#include <libc.h>

#include "uart.h"
#include "log.h"

u32 argc;
char *argv[5] = {NULL};

s32 shell(char *cmd)
{
    PRINT_DEBUG("recv cmd %s \n", cmd);
    return 0;
}
