#include <stdio.h>
#include <stack.h>
#include <error_detection.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "Errors.h"
#include "asm.h"
#include "cpu.h"
#include "asm_commands.h"
#include "double_cmp.h"

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        printf ("Invalid CPU call\n");
        return -1;
    }
    FILE* program_file = fopen (argv[1], "r");
    if (program_file == NULL)
    {
        printf ("File opening error\n");
        return -1;
    }
    
    enum CPU_Errors result = CPU_Errors_NICE;
    ssize_t cmd_amount = 0;
    result = preprocessing_check (program_file, &cmd_amount);
    if (result != CPU_Errors_NICE)
    {
        error_out (result);
        return -1;
    }
    
    num_type* cmd_arr = read_file (program_file, cmd_amount);
    
    cmd_processing (cmd_arr, cmd_amount);

    free (cmd_arr);
    return 0;
}

enum CPU_Errors preprocessing_check (FILE* program_file, ssize_t* cmd_amount)
{
    struct compile_info c_info;
    size_t get_signature = 0;
    size_t get_version = 0;

    fread ((void*) &get_signature, sizeof (size_t), 1, program_file);
    fread ((void*) &get_version, sizeof (size_t), 1, program_file);
    fread ((void*) &c_info.size, sizeof (size_t), 1, program_file);

    if (get_signature != c_info.signature)
    {
        return CPU_Errors_INVALID_FFORMAT;
    }
    
    if (get_version != c_info.version)
    {
        return CPU_Errors_INVALID_VERSION;
    }
    
    *cmd_amount = (ssize_t) c_info.size;
    
    return CPU_Errors_NICE;
}

num_type* read_file (FILE* program_file, ssize_t cmd_amount)
{
    assert (program_file);
    
    num_type* cmd_arr = (num_type*) calloc ((size_t) cmd_amount, sizeof (num_type));
    if (cmd_arr == NULL)
        return NULL;
    
    fseek(program_file, 3*sizeof (size_t), SEEK_SET);
    fread ((void*) cmd_arr, sizeof (num_type), (size_t) cmd_amount, program_file);
    
    return cmd_arr;
}

enum CPU_Errors cmd_processing (num_type* cmd_arr, ssize_t cmd_amount)
{
    log_file_Ctor ();
    Stack stk = {};
    stack_ctor (&stk, cmd_amount) ASSERTED;
    
#ifdef DEBUG_CPU
    FILE* CPU_log = fopen ("CPU_log.txt", "w");
    if (CPU_log == NULL)
    {
        printf ("Creating CPU log file error.\n");
        return CPU_Errors_LOG_OPEN_ERR;
    }
#endif
    
    num_type regs[5] = {0};         //--CPU registers--
                                    //  1   2   3   4
                                    // rax rbx rcx rdx
    
    num_type RAM[RAM_SIZE] = {0};   //---RAM array---
    
    Stack call_stk = {};
    stack_ctor (&call_stk, 0) ASSERTED;
    
    num_type value1 = 0;
    num_type value2 = 0;
    
    #define DEF_CMD(name, num, arg, ...) \
    case num:\
        __VA_ARGS__\
        break;
    
    for (ssize_t ip = 0; ip < cmd_amount && (int) cmd_arr[ip] != 0; ip++)
    {
        int cur_cmd = (int) cmd_arr[ip];
        
#ifdef DEBUG_CPU
        cmd_dump (cur_cmd, CPU_log);
#endif
        
        switch (cur_cmd & 0x1F) //mask for the right 5 bits
        {
            /**/
            #include "cmd.h"
            /**/
            default:
            {
                stack_dtor (&stk) ASSERTED;
                stack_dtor (&call_stk) ASSERTED;
                log_file_Dtor ();
                printf ("Unknwown command \"%d\". Emergency shutdown\n", (int) cmd_arr[ip]);
                return CPU_Errors_UNKNOWN_COMMAND;
            }
        }
            
    }
    
#ifdef DEBUG_CPU
    fclose (CPU_log);
#endif
    
    #undef DEF_CMD
    stack_dtor (&stk) ASSERTED;
    stack_dtor (&call_stk) ASSERTED;
    log_file_Dtor ();
    
    return CPU_Errors_NICE;
}

void error_out (enum CPU_Errors result)
{
    switch ((int) result)
    {
        case CPU_Errors_INVALID_FFORMAT:
            printf ("Invalid file format. Please, enter the file processed by asm\n");
            break;
        case CPU_Errors_INVALID_VERSION:
            printf ("The file version does not match the cpu version. Please, recompile the source code\n");
            break;
        default:
            printf ("Unknwown error\n");
            break;
    }
}

void dump_func (num_type* arr, size_t cmd_amount, size_t cur_ip, const Stack* stk)
{
    for (size_t i = 0; i < cmd_amount; i++)
    {
        printf ("%02lu ", i);
    }
    putchar ('\n');
    
    for (size_t i = 0; i < cmd_amount; i++)
    {
        printf ("%2.0lf ", arr[i]);
    }
    putchar ('\n');
    
    for (size_t i = 0; i < cur_ip; i++)
    {
        for (int j = 0; j < 3; j++)
            putchar ('_');
    }
    putchar ('^');
    printf ("IP = %lu\n", cur_ip);
    
    for (ssize_t i = 0; i < (stk -> size); i++)
    {
        printf ("[%ld] = " NUM_FMT " \n", i, (stk -> data)[i]);
    }
}
