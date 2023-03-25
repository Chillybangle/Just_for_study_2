/*!
 * \file
 * \brief Header with descriptions of used in this project debug functions
 * 
 * This file contains definitions of all debug functions which were used
 * in the "Stack" project
 */

#include "stack.h"
#ifndef ERRORDETHEADER
#define ERRORDETHEADER

/// Modes of the verifier function
enum Mode
{
    Mode_CHECK,
    Mode_RECOUNT
};

const unsigned int POISON = 0xBADDED32; //< The trash value which puts in the array cell when it did its work

/*!
 * \brief prints information that an error has occured in the log_file
 * 
 * \param[in] path_to_file full path to the file where an error has occured 
 * \param[in] line number of the line where an error has occured 
 * 
 * \return -1 if the log_file variable is a NULL pointer and 0 if success
 */
int Error (const char* path_to_file, const int line);

/*!
 * \brief opens the log_file (its a FILE* variable which points to the file where will the logs be written)
 */
void log_file_Ctor ();

/*!
 * \brief closes the log_file
 */
void log_file_Dtor ();

/*!
 * \brief checks if the stack pointer stk is NULL
 * Writes in the log file about it if it is so
 * 
 * \param[in] stk pointer to the struct stack variable
 * 
 * \return errors
 */
enum Stack_Errors stack_error (Stack* stk);

/*!
 * \brief Checks if the value pointer is NULL
 * 
 * \param[in] value pointer to the value
 * 
 * \return errors
 */
enum Stack_Errors value_error (elem_type* value);

/*!
 * \brief Checks the fields of the stack struct to the correct values
 * 
 * In the Debug mode it also calls hash functions and checks the canaries
 * 
 * \param[in] stk pointer to the struct stack variable
 * \param[in] Mode operating mode of verifier (Mode_CHECK to check values of hash and canaries and Mode_RECOUNT to recount them after operations with stack)
 * 
 * \return stack errors 
 */
enum Stack_Errors verifier (Stack* stk, enum Mode Mode);

/*!
 * \brief prints logs to the file
 * 
 * Gets errors of the stack and print info about them. Can print content of the stack too.
 * Also if prints debugging info about calling function etc.
 * 
 * \param[in] stk pointer to the struct stack variable
 * \param[in] result stack errors
 * \param[in] func name of the calling function 
 * \param[in] file name of the calling file
 * \param[in] line number of the line from which the function was called
 */
void stack_dump_func (Stack* stk, enum Stack_Errors result, const char* func, const char* file, const int line);

/*!
 * \brief prints errors transcripts to the log_file
 * 
 * \param[in] result stack errors
 * 
 * \return result variable
 */
enum Stack_Errors print_error (enum Stack_Errors result);

/*!
 * \brief counts hash of struct fields
 * 
 * \param[in] struct_ptr pointer to the struct
 * 
 * \return hash
 */
unsigned int struct_hash_counter (void* struct_ptr);

/*!
 * \brief counts hash of the mass
 * 
 * \param[in] data pointer to the array
 * \param[in] n_elems number of elements in the array
 * \param[in] elem_size size of one element of the array
 * 
 * \return hash of the array
 */
unsigned int hash_ly (void* data, size_t n_elems, size_t elem_size);


///debug templates
#if DEBUG>0
    #define STACK_DUMP(stk, result) stack_dump_func(stk, result, __PRETTY_FUNCTION__, __FILE__, __LINE__) //< Stack_Dump template 
    #define ASSERT_OK(stk, result) if(stack_error (stk) == Stack_Errors_FINE) STACK_DUMP(stk, result) //< Correct call of stack dump function
    #define ASSERTED == Stack_Errors_FINE || Error(__FILE__, __LINE__) //< Chech if the func returns error. If it's so then print about it to the logs
    #define FPRINT_ERR(ERROR, TEXT) if (result & ERROR) fprintf (log_file, TEXT); fflush (log_file) //< template to print_error function
    #define DATA_ADDRESSING_CHECK(result) !(result & Stack_Errors_OVERWRITING_ATTEMPT) && !(result & Stack_Errors_REALLOC_FAILED) && !(result & Stack_Errors_DATA_NULL_PTR) && !(result & Stack_Errors_NEGATIVE_CAPACITY) && !(result & Stack_Errors_TOO_LARGE_SIZE) && !(result & Stack_Errors_NEGATIVE_SIZE) && !(result & Stack_Errors_ZERO_CAPACITY) && !(result & Stack_Errors_VALUE_NULL_PTR) && !(result & Stack_Errors_STRUCT_HASH_ERR) //< checks if there are any errors related to stack memory access
#endif

#if DEBUG==0
    #define STACK_DUMP(stk, result) 
    #define ASSERT_OK(stk, result) 
    #define ASSERTED 
    #define FPRINT_ERR(ERROR, TEXT) 
#endif

#endif
