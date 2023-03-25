#ifndef CPU_ERRORS_H
#define CPU_ERRORS_H
enum CPU_Errors: int
{
    CPU_Errors_NICE,
    CPU_Errors_INVALID_FFORMAT,
    CPU_Errors_INVALID_VERSION,
    CPU_Errors_UNKNOWN_COMMAND,
    CPU_Errors_POP_VALUE_ERR,
    CPU_Errors_LOG_OPEN_ERR,
    CPU_Errors_IN_ERR
};
#endif
