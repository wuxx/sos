#include "vsprintf.h"

char buf[100];

int main()
{
    snprintf(buf, 100, "hello,world, %s, %d\n", "abc12", 100);
    printf("%s", buf);
    return 0;
}

