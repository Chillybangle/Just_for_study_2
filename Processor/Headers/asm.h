#ifndef ASM_H
#define ASM_H
/// a structure which contains a pointer to a string and its size
struct str_pointer
{
    char* p = NULL; //< string pointer
    size_t size = 0; //< size of string
};

struct compile_info
{
    size_t signature = 0xDEADFFFF;
    size_t version = 1;
    size_t size = 0;
};

typedef double num_type;
#define NUM_FMT "%lf"
const size_t MAX_LABELS_NUM = 50;
const size_t MAX_LABEL_SIZE = 50;
const size_t LABEL_POISON = 100500;

const int ARG_IMMED = 0x20;
const int ARG_REG = 0x40;
const int ARG_RAM = 0x80;

const int RAM_SIZE = 400;
const int VIDEO_MEM_START = 100;

struct Labels
{
    size_t address;
    char label_name [MAX_LABEL_SIZE];
};

size_t fsize (char* name);
struct str_pointer* read_source (char* source_name, size_t* nlines, char** array_text);
size_t nstring_counter (const char* array, size_t size);
int array_p_make (struct str_pointer* *array_of_pointers, char* array_text, size_t nstrings);
int convert_to_asm (struct str_pointer* array_line_ptrs, size_t nlines, num_type* result_arr, struct compile_info* c_info, struct Labels* labels);
int convert_to_asm_in_txt (struct str_pointer* array_line_ptrs, size_t nlines, const char* txt_name);
int bin_file_out (num_type* result_arr, struct compile_info c_info, const char* bin_file_name);
int stricmp (const char* string1, const char* string2);
int space_symbols (const char* string);
void comment_pass (char* string);
size_t find_label (struct Labels* labels, char* read_label);
struct Labels* labels_make();
void labels_delete (struct Labels* labels);
int label_record (const char* cmd, struct Labels* labels, size_t commands_amount);
int colon_find (const char* string);
int get_args (char* string, int nread_symb, num_type* val, int* add_nread, int* cmd_flags);
int get_args_jmp (char* string, int nread_symb, num_type* val, int* add_nread, int compile_number, struct Labels* labels);
int determ_arg (char* str, int* cmd_flags, num_type* result);

#define TXT_OUT
#ifdef TXT_OUT
    #define TXT_FILE_PRINT1(CMD) fprintf (txt_file, "%d\n", CMD)
    #define TXT_FILE_PRINT2(CMD, specifier) fprintf (txt_file, "%d " specifier " \n", CMD, val)
#endif



#endif
