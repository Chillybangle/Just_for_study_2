#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "error_detection.h"
#include "stack.h"

int is_equal (elem_type num_1, elem_type num_2);

static FILE* log_file = NULL;

int Error (const char* path_to_file, const int line)
{
    assert (path_to_file);
    
    if (log_file != NULL)
    {
        const char* file_name = strrchr(path_to_file, '/');
        if (file_name == NULL)
            file_name = path_to_file;
        fprintf (log_file, "Error: Function in \"%s\" file on (%d) line returns non-zero value\n", file_name, line);
        
        return 0;
    }
    
    return -1;
}

void log_file_Ctor ()
{
    log_file = fopen("log.txt", "w");
    if (log_file == NULL)
    {
        printf ("Cant create log file\n");
        exit(1);
    }
}

void log_file_Dtor ()
{
    fclose (log_file);
    log_file = NULL;
}

enum Stack_Errors stack_error (Stack* stk)
{
    if (stk == NULL)
    {
        fprintf (log_file, "Error: pointer to Stack struct is a NULL-pointer\n");
        
        return Stack_Errors_STACK_NULL_PTR;
    }
    
    return Stack_Errors_FINE;
}
enum Stack_Errors value_error (elem_type* value)
{
    if (value == NULL)
    {
        fprintf (log_file, "Error: pointer to value to stack_pop function is NULL\n");
        
        return Stack_Errors_VALUE_NULL_PTR;
    }
    
    return Stack_Errors_FINE;
}

enum Stack_Errors verifier (Stack* stk, enum Mode Mode)
{
    enum Stack_Errors result = Stack_Errors_FINE;
    
    if (stk == NULL)
    {
        return Stack_Errors_STACK_NULL_PTR;
    }
    else
    {
        if (stk -> data == NULL)
            result = (Stack_Errors) (result | Stack_Errors_DATA_NULL_PTR);
        if (stk -> size >= stk -> capacity)
            result = (Stack_Errors) (result | Stack_Errors_TOO_LARGE_SIZE);
        if (stk -> capacity == -1)
            result = (Stack_Errors) (result | Stack_Errors_NEGATIVE_CAPACITY);
        if (stk -> size == -1)
            result = (Stack_Errors) (result | Stack_Errors_NEGATIVE_SIZE);
#if DEBUG>2
        //static unsigned int struct_hash = 0;
        if (Mode == Mode_RECOUNT)
        {
            stk -> struct_hash = struct_hash_counter ((void*) stk);
        }
        else
        {
            if (stk -> struct_hash != struct_hash_counter ((void*) stk))
                result = (Stack_Errors) (result | Stack_Errors_STRUCT_HASH_ERR);
        }
#endif
        
        if (result != Stack_Errors_FINE)
            return result;
    
#if DEBUG>2
        if (Mode == Mode_RECOUNT && result == Stack_Errors_FINE)
        {
            *(unsigned int*) (stk -> data + stk -> capacity) = hash_ly ((void*) (stk -> data), (size_t) (stk -> capacity), sizeof(stk -> data[0]));
        }
        else if (result == Stack_Errors_FINE)
        {
            if (hash_ly ((void*) (stk -> data), (size_t) (stk -> capacity), sizeof(stk -> data[0])) != *(unsigned int*) (stk -> data + stk -> capacity))
                result = (Stack_Errors) (result | Stack_Errors_HASH_MISMATCH);
        }
        else
        {
            result = (Stack_Errors) (result | Stack_Errors_HASH_WARNING);
        }
#endif

#if DEBUG==2 || DEBUG==4
        if (Mode == Mode_CHECK)
        {
            if (stk -> left_canary != DEAD_CONST)
                result = (Stack_Errors) (result | Stack_Errors_LEFT_CANARY_ERR);
            if (stk -> right_canary != DEAD_CONST)
                result = (Stack_Errors) (result | Stack_Errors_RIGHT_CANARY_ERR);
            if (*((canary_type*) stk -> data - 1) != DEAD_CONST)
                result = (Stack_Errors) (result | Stack_Errors_DATA_LCANARY_ERR);
            
#if DEBUG==2
        if (Mode == Mode_CHECK && result == Stack_Errors_FINE)
            if (*(unsigned int*) (stk -> data + stk -> capacity) != DEAD_CONST)
                result = (Stack_Errors) (result | Stack_Errors_DATA_RCANARY_ERR);
#endif
        }
#endif
    }
    
    return result;
}
#if DEBUG>0
void stack_dump_func (Stack* stk, enum Stack_Errors result, const char* func, const char* file, const int line)
{
    assert (stk);
    
    if (func != NULL && file != NULL)
    {
        fprintf (log_file, "\n%s at %s(%d):\n", func, file, line); // fvprintf
        fflush (log_file);
    }
    else
        fprintf (log_file, "\nstack_dump_func failed: unknown calling function or (and) file\n");
    fflush (log_file);
    
    if ((stk -> initialization.constructed) == true)
    {
        if (stk -> initialization.name != NULL && stk -> initialization.call_func != NULL && 
            stk -> initialization.call_file != NULL)
        {
#ifdef LOGALL
            fprintf (log_file, "Stack[%p] (%s) \"%s\" at %s at %s(%ld)\n", 
                     stk, (result == Stack_Errors_FINE) ? "ok" : "error",
                     stk -> initialization.name, stk -> initialization.call_func,
                     stk -> initialization.call_file, stk -> initialization.line);
            fflush (log_file);
            if (result != Stack_Errors_FINE)
            {
                print_error (result);
            }
            fprintf (log_file,  "\t{\n"
                                "\t\tsize = %ld\n"
                                "\t\tcapacity = %ld\n"\
                                "\t\tdata[%p]\n"
                                "\t\t\t{\n",
                                stk -> size, stk -> capacity, stk);
            fflush (log_file);
            
            if (DATA_ADDRESSING_CHECK(result))
            {
                ssize_t i = 0;
                for (; i < stk -> size; i++)
                {
                    fprintf (log_file, "\t\t\t\t*[%ld] = " ELEM_FMT " %s\n", i, stk -> data[i], (is_equal (stk -> data[i], (elem_type) POISON)) ? "(POISON)" : "");
                    fflush (log_file);
                }
                for (; i < stk -> capacity; i++)
                {
                    fprintf (log_file, "\t\t\t\t[%ld] = " ELEM_FMT " %s\n", i, stk -> data[i], (is_equal (stk -> data[i], (elem_type) POISON)) ? "(POISON)" : "");
                    fflush (log_file);
                }
            
                fprintf (log_file, "\t\t\t}\n"
                                    "\t}\n");
                fflush (log_file);
            }
#endif
            
#ifndef LOGALL
            fprintf (log_file, "Stack[%p] (%s) \"%s\" at %s at %s(%ld)\n", 
                     stk, (result == Stack_Errors_FINE) ? "ok" : "error",
                     stk -> initialization.name, stk -> initialization.call_func,
                     stk -> initialization.call_file, stk -> initialization.line);
            fflush (log_file);
            if (result != Stack_Errors_FINE)
            {
                print_error (result);
                
                fprintf (log_file,  "\t{\n"
                                    "\t\tsize = %ld\n"
                                    "\t\tcapacity = %ld\n"\
                                    "\t\tdata[%p]\n"
                                    "\t\t\t{\n",
                                    stk -> size, stk -> capacity, stk);
                fflush (log_file);
                
                if (DATA_ADDRESSING_CHECK(result))
                {
                    ssize_t i = 0;
                    for (; i < stk -> size; i++)
                    {
                        fprintf (log_file, "\t\t\t\t*[%ld] = " ELEM_FMT " %s\n", i, stk -> data[i], (is_equal (stk -> data[i], (elem_type) POISON)) ? "(POISON)" : "");
                        fflush (log_file);
                    }
                    for (; i < stk -> capacity; i++)
                    {
                        fprintf (log_file, "\t\t\t\t[%ld] = " ELEM_FMT " %s\n", i, stk -> data[i], (is_equal (stk -> data[i], (elem_type) POISON)) ? "(POISON)" : "");
                        fflush (log_file);
                    }
                
                    fprintf (log_file, "\t\t\t}\n"
                                        "\t}\n");
                    fflush (log_file);
                }
            }
#endif
        }
        else
        {
            fprintf (log_file, "Stack[%p] (%s) initialize info failed\n", stk, (result == Stack_Errors_FINE) ? "ok" : "error");
            fflush (log_file);
        }
    }
    else 
    {
        fprintf (log_file, "Stack[%p] was not constructed\n", stk);
        fflush (log_file);
    }
}
#endif
enum Stack_Errors print_error (enum Stack_Errors result)
{
    FPRINT_ERR (Stack_Errors_OVERWRITING_ATTEMPT,
                "-Overwriting attempt (this stack has already been created, "
                           "constructor call error)\n");
    FPRINT_ERR (Stack_Errors_REALLOC_FAILED,
                "-Realloc returns NULL-pointer (stack resize error)\n");
    FPRINT_ERR (Stack_Errors_STACK_NULL_PTR,
                "-The pointer to a struct Stack variable is a NULL-pointer\n");
    FPRINT_ERR (Stack_Errors_DATA_NULL_PTR,
                "-The buffer pointer (data) is a NULL-pointer\n");
    FPRINT_ERR (Stack_Errors_TOO_LARGE_SIZE,
                "-A wild situation with Stack: size >= capacity\n");
    FPRINT_ERR (Stack_Errors_NEGATIVE_CAPACITY,
                "-Stack capacity is negative! (impossible value)\n");
    FPRINT_ERR (Stack_Errors_NEGATIVE_SIZE,
                "-Stack size is negative! (impossible value)\n");
    FPRINT_ERR (Stack_Errors_INVALID_POP,
                "-Invalid pop. Attempt to pull out an element with a negative number\n");
#if DEBUG!=4
    FPRINT_ERR (Stack_Errors_HASH_MISMATCH,
                "-Hash mismatch. Current value does not match the previous one\n");
#endif
    
#if DEBUG==4
    FPRINT_ERR (Stack_Errors_HASH_MISMATCH,
                "-Hash mismatch. Current value does not match the previous one or right data canary was attacked\n");
#endif
    FPRINT_ERR (Stack_Errors_HASH_WARNING,
                "-The hash was not calculated or verified due to other errors\n");
    FPRINT_ERR (Stack_Errors_LEFT_CANARY_ERR,
                "-Left canary const in the struct was changed!\n");
    FPRINT_ERR (Stack_Errors_RIGHT_CANARY_ERR,
                "-Right canary const in the struct was changed!\n");
    FPRINT_ERR (Stack_Errors_DATA_LCANARY_ERR,
                "-Left canary in the stack data was changed!\n");
    FPRINT_ERR (Stack_Errors_DATA_RCANARY_ERR,
                "-Right canary in the stack data was changed!\n");
    FPRINT_ERR (Stack_Errors_STRUCT_HASH_ERR,
                "-Some element in the stack struct (data, size or capacity) was changed!\n");
    
    return result;
}

unsigned int struct_hash_counter (void* struct_ptr)
{
    Stack* stack_ptr = (Stack*) struct_ptr;
    
    unsigned int hash = 0;
    
    hash = (hash * 1664525) + (unsigned int) (uintptr_t) (stack_ptr -> data) + 1013904223;
    hash = (hash * 1664525) + (unsigned int) stack_ptr -> size + 1013904223;
    hash = (hash * 1664525) + (unsigned int) stack_ptr -> capacity + 1013904223;
           
    return hash;
}

unsigned int hash_ly (void* data, size_t n_elems, size_t elem_size)
{
    unsigned int hash = 0;
    size_t size = elem_size * n_elems;
    char * data_ch = (char*) data;

    for(size_t i = 0; i < size; i++)
    {
        hash = (hash* 1664525) + *(unsigned char*) data_ch + 1013904223;
        data_ch++;
    }

    return hash;
}

int is_equal (elem_type num_1, elem_type num_2)
{
    if (fabs(num_1 - num_2) < float_cmp_accuracy)
        return 1;
    return 0;
}
