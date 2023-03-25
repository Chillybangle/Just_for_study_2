#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Akinator.h"
#include "error_detection.h"

static void Make_Poisoned (Stack* stk);

enum Stack_Errors stack_ctor_func (Stack* stk, ssize_t capacity, const char* name, const char* call_func, const char* call_file, ssize_t line)
{
    enum Stack_Errors result = verifier (stk, Mode_RECOUNT);
    ASSERT_OK (stk, result);
    assert (!stack_error (stk));
    
    if (stk -> data == NULL)
    {
        if (capacity == -1)
            result = (Stack_Errors) (result | Stack_Errors_NEGATIVE_CAPACITY);
        else
        {
            if (capacity == 0)
                capacity = MIN_CAPACITY;
#if DEBUG==0 || DEBUG==1
            stk -> data = (elem_type*) calloc ((size_t) capacity, sizeof (elem_type));
#endif
            
#if DEBUG>1
            stk -> data = (elem_type*) calloc ((size_t) capacity + 2 * sizeof(canary_type), sizeof (elem_type));
            *(canary_type*) stk -> data = DEAD_CONST;
            stk -> data = (elem_type*) ((canary_type*) stk -> data + 1); // canary type
            *(canary_type*) (stk -> data + capacity) = DEAD_CONST;
#endif
            stk -> capacity = capacity;
        }
    }
    else
        result = (Stack_Errors) (result | Stack_Errors_OVERWRITING_ATTEMPT);
    
#if DEBUG>0
    stk -> initialization.constructed = true;
    stk -> initialization.name = name;
    stk -> initialization.call_func = call_func;
    stk -> initialization.call_file = call_file;
    stk -> initialization.line = line;
#endif
    
    result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
    ASSERT_OK(stk, result);
    
    return result;
}

enum Stack_Errors stack_dtor (Stack* stk)
{
    assert (!stack_error (stk));
    if (stk -> data == NULL)
        return Stack_Errors_DATA_NULL_PTR;
    enum Stack_Errors result = verifier (stk, Mode_CHECK);
    ASSERT_OK (stk, result);
    FAIL_RETURN (result);
    
    if (stk -> capacity != -1)
    {
        stk -> size = 0;
        Make_Poisoned (stk);
    }
    stk -> size = -1;
    stk -> capacity = -1;
    
#if DEBUG>1
    if ((elem_type*) ((canary_type*) stk -> data - 1) == NULL)
        return Stack_Errors_DATA_NULL_PTR;
    free ((elem_type*) ((canary_type*) stk -> data - 1));
#endif
    
#if DEBUG==0 || DEBUG==1
    if (stk -> data == NULL)
        return Stack_Errors_DATA_NULL_PTR;
    free (stk -> data);
#endif
    
    stk -> data = NULL;
    
    result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
    ASSERT_OK (stk, result);
    
    return result;
}

enum Stack_Errors stack_push (Stack* stk, elem_type value)
{
    enum Stack_Errors result = verifier (stk, Mode_CHECK);
    ASSERT_OK (stk, result);
    FAIL_RETURN (result);
    assert (!stack_error (stk));
    
    if ((stk -> size + 1) >= stk -> capacity)
        result = (Stack_Errors) (result | stack_resize (stk, (stk -> capacity) * 2));
    stk -> data [stk -> size++] = value;
    
    result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
    ASSERT_OK(stk, result);
    
    return result;
}

enum Stack_Errors stack_resize (Stack* stk, ssize_t capacity)
{
    enum Stack_Errors result = verifier (stk, Mode_CHECK);
    ASSERT_OK (stk, result);
    FAIL_RETURN (result);
    assert (!stack_error (stk));
    
    if (capacity < 1)
        return print_error (Stack_Errors_ZERO_CAPACITY);
    if (capacity >= MIN_CAPACITY)
    {
        stk -> capacity = capacity;
        
#if DEBUG==0 || DEBUG==1
        stk -> data = (elem_type*) realloc (stk -> data, (size_t) capacity * sizeof (elem_type));
#endif
        
#if DEBUG>1
        stk -> data = (elem_type*) realloc ((elem_type*) ((canary_type*) stk -> data - 1), (size_t) capacity * sizeof (elem_type) + 2 * sizeof (canary_type));
        *(canary_type*) stk -> data = DEAD_CONST;
        stk -> data = (elem_type*) ((canary_type*) stk -> data + 1);
        *(canary_type*) (stk -> data + capacity) = DEAD_CONST;
        Make_Poisoned (stk);
#endif
    }
    if (stk -> data == NULL)
        return print_error (Stack_Errors_REALLOC_FAILED);
    
    result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
    ASSERT_OK(stk, result);
    return result;
}

enum Stack_Errors stack_pop (Stack* stk, elem_type* value)
{
    enum Stack_Errors result = verifier (stk, Mode_CHECK);
    ASSERT_OK (stk, result);
    FAIL_RETURN (result);
    assert (!value_error (value));
    
    if (stk -> size > 0)
    {
        *value = stk -> data[--(stk -> size)];
    }
    else
    {
        result = (Stack_Errors) (result | Stack_Errors_INVALID_POP);
        result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
        ASSERT_OK (stk, result);
        return result;
    }
    
    if (stk -> size < stk -> capacity / 4) 
        result = (Stack_Errors) (result | stack_resize (stk, (stk -> capacity) / 2));
    
    stk -> data[(stk -> size)] = (elem_type) POISON;
    result = (Stack_Errors) (result | verifier (stk, Mode_RECOUNT));
    ASSERT_OK (stk, result);
    
    return result;
}

static void Make_Poisoned (Stack* stk)
{
    ssize_t capacity = stk -> capacity;
    for (ssize_t i = stk -> size; i < capacity; i++)
        stk -> data[i] = (elem_type)POISON;
}
