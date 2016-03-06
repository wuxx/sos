#include <types.h>
#include "int.h"

/* __aeabi_ldiv0 will call raise */
s32 raise(s32 signum)
{
    lockup();
    return 0;
}
