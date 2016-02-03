#include <libc.h>
#include "cpu.h"

/* task state */
#define TASK_UNUSED  0
#define TASK_RUNNING 1
#define TASK_SUSPEND 2
#define TASK_READY   3

struct __task__
{
    u32 sp; 
    u32 id;
    u32 state;
    void *stack;
    u32 stack_size;
    func_1 entry;
};
