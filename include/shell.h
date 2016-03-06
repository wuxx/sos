#include <types.h>

#define SHELL_ARGS_MAX  (5)
struct shell_cmd_info {
    char *name;
    func_0 func;
    char *desc;
};

s32 shell(char *cmd);
