/*!
 * \file
 * \brief Header with descriptions of used in this project functions
 * 
 * This file contains definitions of all main functions which were used
 * in the "Stack" project
 */

#define DEBUG 0
//#define LOGALL
//0 - without debug
//1 - debug
//2 - debug with canaries
//3 - debug with hash
//4 - debug with canaries and hash
//LOGALL - print all logs which are possible

#include <stdbool.h>
#ifndef STACKSTRUCT
#define STACKSTRUCT

typedef struct Descrip elem_type;          //< type of element in the stack
#define ELEM_FMT "%lf"           //< specifier of this type to printf
const ssize_t MIN_CAPACITY = 5; //< min capacity of created stack
const double float_cmp_accuracy = 0.01;

/// Constants which show errors (bitmask format)
enum Stack_Errors
{
    Stack_Errors_FINE                = 0b0,
    Stack_Errors_OVERWRITING_ATTEMPT = 0b1,
    Stack_Errors_REALLOC_FAILED      = 0b10,
    Stack_Errors_STACK_NULL_PTR      = 0b100,
    Stack_Errors_DATA_NULL_PTR       = 0b1000,
    Stack_Errors_TOO_LARGE_SIZE      = 0b10000,
    Stack_Errors_NEGATIVE_CAPACITY   = 0b100000,
    Stack_Errors_NEGATIVE_SIZE       = 0b1000000,
    Stack_Errors_ZERO_CAPACITY       = 0b10000000,
    Stack_Errors_VALUE_NULL_PTR      = 0b100000000,
    Stack_Errors_INVALID_POP         = 0b1000000000,
    Stack_Errors_HASH_MISMATCH       = 0b10000000000,
    Stack_Errors_HASH_WARNING        = 0b100000000000,
    Stack_Errors_LEFT_CANARY_ERR     = 0b1000000000000,
    Stack_Errors_RIGHT_CANARY_ERR    = 0b10000000000000,
    Stack_Errors_DATA_LCANARY_ERR    = 0b100000000000000,
    Stack_Errors_DATA_RCANARY_ERR    = 0b1000000000000000,
    Stack_Errors_STRUCT_HASH_ERR     = 0b10000000000000000
};

#if DEBUG>0
typedef struct
{
    bool constructed = false;
    const char* name = NULL;
    const char* call_func = NULL;
    const char* call_file = NULL;
    ssize_t line = -1;
} Varinfo;

typedef elem_type canary_type;          //< type of canaries
const canary_type DEAD_CONST = 0x1DEADFFF; //< const to canaries
#endif

typedef struct stack
{
#if DEBUG==2 || DEBUG==4
    canary_type left_canary = DEAD_CONST;
#endif
    
    elem_type* data = NULL;
    ssize_t size = 0;
    ssize_t capacity = 0;
    
#if DEBUG>0
    Varinfo initialization = {};
#endif
    
#if DEBUG==2 || DEBUG==4
    canary_type right_canary = DEAD_CONST;
#endif
    
#if DEBUG>2
    unsigned int struct_hash = 0;
#endif
} Stack;

/*!
 * \brief constructor of the stack
 * 
 * \param[in] stk pointer to stack variable
 * \param[in] capacity capacity of created stack
 * \param[in] name (debugging information) pointer to the name of stack variable to which the pointer was passed
 * \param[in] call_func (debugging information) pointer to the name of call function from which the constructor was called
 * \param[in] call_file (debugging information) pointer to the name of call file from which the constructor was called
 * \param[in] line (debugging information) line number from which the constructor function was called
 * 
 * \return errors which occurred in the function
 */
enum Stack_Errors stack_ctor_func (Stack* stk, ssize_t capacity, const char* name, const char* call_func, const char* call_file, ssize_t line);

/*!
 * \brief template to stack constructor function without debugging params
 * 
 * Allocates memory for the data pointer. Initially data pointer must be NULL! Writes the capacity of allocated memory to stk.capacity.
 * 
 * \param[in] stk pointer to struct stack variable
 * \param[in] capacity capacity of created stack
 * 
 * \return errors which occurred in the function
 */
#define stack_ctor(stk, capacity) stack_ctor_func(stk, capacity, #stk + (#stk[0] == '&'), __PRETTY_FUNCTION__, __FILE__, __LINE__)

/*!
 * \brief template to make a return from the function after the initial check if errors detected (the function will not do its work)
 */
#define FAIL_RETURN(result) if(result != Stack_Errors_FINE) return result

/*!
 * \brief push the element in the stack
 * 
 * \param[in] stk pointer to the struct stack variable
 * \param[in] value pushed value of elem_type type
 * 
 * \return errors which occurred in the function
 */
enum Stack_Errors stack_push (Stack* stk, elem_type value);

/*!
 * \brief pop the element from the stack
 * 
 * \param[in] stk pointer to the struct stack variable
 * \param[in] value pointer to the variable of elem_type type where will the value be put 
 * 
 * \return errors which occurred in the function
 */
enum Stack_Errors stack_pop (Stack* stk, elem_type* value);

/*!
 * \brief resize the stack capacity
 * 
 * \param[in] stk pointer to the struct stack variable
 * \param[in] capacity new capacity of the stack
 * 
 * \return errors which occurred in the function
 */
enum Stack_Errors stack_resize (Stack* stk, ssize_t capacity);

/*!
 * \brief free the stack and fill the stack variables in invalid values
 * 
 * \param[in] stk pointer to the struct stack variable
 * \return errors which occurred in the function
 */
enum Stack_Errors stack_dtor (Stack* stk);

#endif
