#ifndef ASM_COMMANDS
#define ASM_COMMANDS

#define DEF_CMD(name, num, arg, ...) \
    CMD_##name = num,

enum Commands // commands can be up to the number 31 otherwise collisions
{             // with RAM and register bits will begin
#include "cmd.h"
};

#undef DEF_CMD
#endif
