#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "asm_commands.h"
#include "asm.h"
#include "double_cmp.h"

const char* DEFAULT_TXT_NAME = "../a.txt";
const char* DEFAULT_BIN_NAME = "../b.bin";
const char* LABEL_STR_POISON = "__NOLABEL__";

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        if (argc != 3)
        {
            printf ("Input the source file name as command line argument (+optional: name of compiled file)\n");
            return -1;
        }
    }
    
    size_t nlines = 0;
    
    char* array_text = NULL;
    struct str_pointer* array_line_ptrs = read_source (argv[1], &nlines, &array_text);
    num_type* result_arr = (num_type*) calloc (nlines * 2, sizeof (num_type));
    
    struct compile_info c_info;
    
    int convert_result = 0;
    struct Labels* labels = labels_make();
    
    convert_result = convert_to_asm (array_line_ptrs, nlines, result_arr, &c_info, labels);
    convert_result = convert_to_asm (array_line_ptrs, nlines, result_arr, &c_info, labels);
    
    if (convert_result == 0)
    {
        if (argc == 3)
            bin_file_out (result_arr, c_info, argv[2]);
        else
            bin_file_out (result_arr, c_info, NULL);
    }
    else
    {
        printf ("Compilation failed. File was not created\n");
    }
    
    labels_delete (labels);
    free (array_line_ptrs);
    free (result_arr);
    free (array_text);
}

struct str_pointer* read_source (char* source_name, size_t* nlines, char** array_text)
{
    FILE* input_stream = fopen (source_name, "r");
    if (input_stream == NULL)
    {
        perror ("Error: I can't open a file with this name");
        return NULL;
    }
    
    size_t size = fsize (source_name);
    if (size == 0)
    {
        printf ("Error: I can't get the file size\n");
        return NULL;
    }
    
    *array_text = (char*) calloc (size + 1, sizeof (char)); // \0 symbol in the end
    
    fread (*array_text, sizeof(char), size, input_stream);
    if (ferror (input_stream))
    {
        printf ("Error: I can't read the file\n");
        return NULL;
    }
    
    
    *nlines = nstring_counter (*array_text, size);
    
    struct str_pointer* array_p = NULL;
    if (array_p_make (&array_p, *array_text, *nlines) == -1)
        return NULL;
    
    fclose (input_stream);
    
    return array_p;
}

int convert_to_asm (struct str_pointer* array_line_ptrs, size_t nlines, num_type* result_arr, struct compile_info* c_info, struct Labels* labels)
{
    static int compile_number = 0;
    
    //61 wtf?
    char cmd[61] = "";
    size_t lcounter = 0;
    
    int nread_symb = 0;
    int add_nread = 0;
    int cmd_flags = 0;
    
    num_type val = 0;
    size_t commands_amount = 0;
    
#ifdef TXT_OUT
    FILE* txt_file = NULL;
    txt_file = fopen (DEFAULT_TXT_NAME, "w");
#endif
    
    #define DEF_CMD(name, num, arg, ...) \
        if (stricmp(cmd, #name) == 0) \
        {\
            if (arg)\
            {\
                switch (num)\
                {\
                    case 1: case 2:/*in case push or pop command*/\
                        if (get_args (array_line_ptrs[lcounter].p, nread_symb, &val, &add_nread, &cmd_flags) != 0)\
                        {\
                            printf ("Wrong argument \'%s\' of" #name "function in \'%s\' line\n", array_line_ptrs[lcounter].p + nread_symb, array_line_ptrs[lcounter].p);\
                            return -1;\
                        }\
                        *result_arr++ = CMD_##name | cmd_flags;\
                        *result_arr++ = val;\
                        commands_amount += 2;\
                        TXT_FILE_PRINT2 (CMD_##name | cmd_flags, NUM_FMT);\
                        \
                        val = 0;\
                    break;\
                    \
                    default:/*in case jmp command*/\
                        if (get_args_jmp (array_line_ptrs[lcounter].p, nread_symb, &val, &add_nread, compile_number, labels) != 0)\
                        {\
                            printf ("Wrong argument \'%s\' of" #name "function in \'%s\' line\n", array_line_ptrs[lcounter].p + nread_symb, array_line_ptrs[lcounter].p);\
                            return -1;\
                        }\
                        *result_arr++ = CMD_##name;\
                        *result_arr++ = val;\
                        commands_amount += 2;\
                        TXT_FILE_PRINT2 (CMD_##name, NUM_FMT);\
                        \
                        val = 0;\
                    break;\
                }\
                \
            }\
            else\
            {\
                *result_arr++ = CMD_##name;\
                commands_amount++;\
                TXT_FILE_PRINT1 (CMD_##name);\
            }\
        }\
        else
            
            
    while (lcounter < nlines)
    {
        int nspace = 0;
        comment_pass (array_line_ptrs[lcounter].p);
        while (isspace((array_line_ptrs[lcounter].p)[nspace]))
            nspace++;
        
        array_line_ptrs[lcounter].p += nspace;
        sscanf (array_line_ptrs[lcounter].p, "%60s %n", cmd, &nread_symb);
        if (array_line_ptrs[lcounter].p[0] == '\0')
        {
            lcounter++;
            continue;
        }
        
        if (label_record (cmd, labels, commands_amount) == 0)
        {
            lcounter++;
            continue;
        }
        
        #include "cmd.h"
        
        {
            printf ("Wrong command \'%s\' in string \'%s\'. Compilation failed.\n", cmd, array_line_ptrs[lcounter].p);
            return -1;
        }
        
        if (space_symbols (array_line_ptrs[lcounter].p + nread_symb + add_nread) != 0)
        {
            printf ("nread_symb = %d add_nread = %d\n", nread_symb, add_nread);
            printf ("Invalid input \'%s\' command in string \'%s\'\n", array_line_ptrs[lcounter].p + nread_symb + add_nread, array_line_ptrs[lcounter].p);
            return -1;
        }
        
        add_nread = 0;
        cmd_flags = 0;
        lcounter++;
    }
    
#ifdef TXT_OUT
    fclose (txt_file);
#endif
    
    #undef DEF_CMD
    compile_number++;
    c_info -> size = commands_amount;
    return 0;
}

int bin_file_out (num_type* result_arr, struct compile_info c_info, const char* bin_file_name)
{
    assert (result_arr);
    
    if (bin_file_name == NULL)
        bin_file_name = DEFAULT_BIN_NAME;
    else
    {
        size_t name_size = strlen (bin_file_name);
        char* temp = (char*) calloc (name_size + 4, sizeof(char));
        temp = strcat (temp, "../");
        bin_file_name = strcat (temp, bin_file_name);
    }
    
    FILE* bin_file = fopen (bin_file_name, "w");
    
    if (bin_file == NULL)
    {
        perror ("Bin file opening error\n");
        return -1;
    }
    
    fwrite ((void*) &c_info, sizeof (c_info), 1, bin_file);
    fwrite ((void*) result_arr, sizeof (num_type), c_info.size, bin_file);
    
    fclose (bin_file);
    
    return 0;
}

int space_symbols (const char* string)
{
    while (*string)
        if (isspace (*string++) == 0)
            return -1;
        
    return 0;
}

void comment_pass (char* string)
{
    char* comment_start_pointer = strchr (string, ';');
    if (comment_start_pointer != NULL)
    {
        *comment_start_pointer++ = '\0'; // the comment in the line starts with ;
        *comment_start_pointer = '\0';
    }
}

struct Labels* labels_make()
{
    struct Labels* labels = (struct Labels*) calloc (MAX_LABELS_NUM + 1, sizeof (struct Labels));
    for (size_t i = 0; i < MAX_LABELS_NUM + 1; i++)
    {
        labels[i].address = LABEL_POISON;
        strncpy (labels[i].label_name, LABEL_STR_POISON, MAX_LABEL_SIZE);
    }

    return labels;
}

void labels_delete (struct Labels* labels)
{
    free (labels);
}

size_t find_label (struct Labels* labels, char* read_label)
{
    for (size_t i = 0; i < MAX_LABELS_NUM; i++)
    {
        if (stricmp(labels[i].label_name, read_label) == 0)
        {
            return labels[i].address;
        }
    }
    
    return LABEL_POISON;
}

int label_record (const char* cmd, struct Labels* labels, size_t commands_amount)
{
    int i = 0;
    while (isspace(cmd[i]))
    {
        i++;
    }
    
    ssize_t cmd_len = colon_find (cmd + i);
    if (cmd_len == -1)
        return -1;
    char* label_name = strndup (cmd + i, (size_t) cmd_len);
    label_name[cmd_len] = '\0';
    
    size_t counter = 0;
    
    for (; counter < MAX_LABELS_NUM; counter++)
    {
        if (stricmp (labels[counter].label_name, LABEL_STR_POISON) == 0 || stricmp (labels[counter].label_name, label_name) == 0)
        {
            if ((size_t) cmd_len + 1 > MAX_LABEL_SIZE)
            {
                printf ("ERROR! Command length > maximum label size\n");
                free (label_name);
                return -1;
            }
            strncpy (labels[counter].label_name, label_name, (size_t) cmd_len + 1);
            labels[counter].address = commands_amount;
            free (label_name);
            
            break;
        }
    }
    
    if (counter == MAX_LABELS_NUM && stricmp(labels[counter].label_name, LABEL_STR_POISON) != 0)
    {
        printf ("Too many labels! Can't create a new one. Change the constant MAX_LABELS_NUM\n");
        return -1;
    }

    return 0;
}

int get_args (char* string, int nread_symb, num_type* val, int* add_nread, int* cmd_flags)
{
    char temp_storage[50] = "";
    if (sscanf (string + nread_symb, NUM_FMT " %n", val, add_nread) == 1)
    {
        *cmd_flags = *cmd_flags | ARG_IMMED;
        return 0;
    }
    if (sscanf (string + nread_symb, "%s %n", temp_storage, add_nread) != 1)
        return -1;
    
    if (temp_storage[0] != '\0')
    {
        int sq_bracket_corr = 0;
        if (temp_storage[0] == '[')
        {
            *cmd_flags = *cmd_flags | ARG_RAM;
            char* sq_br_ptr = strchr (temp_storage + 1, ']');
            if (sq_br_ptr == NULL)
            {
                printf ("Invalid input format\n");
                return -1;
            }
            *sq_br_ptr = '\0';

            sq_bracket_corr++;
        }
        
        num_type result = 0;
        int determ_arg_res = determ_arg (temp_storage + sq_bracket_corr, cmd_flags, &result);
        if (determ_arg_res >= 0)
        {
             if (determ_arg_res == 0)
                 *val = result;
             else
                 *val = determ_arg_res;
        }
        else
        {
            printf ("Determing arguments error\n");
            return -1;
        }
    }
    
    return 0;
}

int determ_arg (char* str, int* cmd_flags, num_type* result)
{
    const int first  = 1;
    const int second = 2;
    const int third  = 3;
    const int fourth  = 4;
    

    if (stricmp (str, "rax") == 0)
    {
        *cmd_flags = *cmd_flags | ARG_REG;
        return first;
    }
    if (stricmp (str, "rbx") == 0)
    {
        *cmd_flags = *cmd_flags | ARG_REG;
        return second;
    }
    if (stricmp (str, "rcx") == 0)
    {
        *cmd_flags = *cmd_flags | ARG_REG;
        return third;
    }
    if (stricmp (str, "rdx") == 0)
    {
        *cmd_flags = *cmd_flags | ARG_REG;
        return fourth;
    }
    
    num_type temp_immediate = -1;
    int temp_add_nread = 0;
    if (sscanf (str, NUM_FMT " %n", &temp_immediate, &temp_add_nread) == 1)
    {
        *cmd_flags = *cmd_flags | ARG_IMMED;
        *result = temp_immediate;
        return 0;
    }
    
    return -1;
}

int get_args_jmp (char* string, int nread_symb, num_type* val, int* add_nread, int compile_number, struct Labels* labels)
{
    int i = 0;
    while (isspace(string[i]))
        i++;

    char read_label[MAX_LABEL_SIZE] = {};
    strncpy (read_label, LABEL_STR_POISON, MAX_LABEL_SIZE);
    size_t where_to_jump = LABEL_POISON;

    if (sscanf (string + i + nread_symb, "%lu %n", &where_to_jump, add_nread) != 1)
    {
        
        if (sscanf (string + i + nread_symb, "%s %n", read_label, add_nread) != 1)
        {
            printf ("Wrong argument \'%s\' of jmp function in \'%s\' line\n", string + i + nread_symb, string);
            return -1;
        }
    }
    
    if (stricmp(read_label, LABEL_STR_POISON) != 0)
    {
        where_to_jump = find_label (labels, read_label);
        if (compile_number != 0 && where_to_jump == LABEL_POISON)
        {
            printf ("Label error: label %s wasn't found\n", read_label);
            return -1;
        }
        
    }
    *val = (double) where_to_jump;
    
    return 0;
}

int colon_find (const char* string)//returns the number of the character in which the colon and checks next symbols (must be only space)
{
    const char* colon_ptr = strchr (string, ':');
    if (colon_ptr == NULL)
    {
        return -1;
    }
    
    if (*(colon_ptr + 1) != '\0')
    {
        for (int i = 1; colon_ptr[i] != '\0'; i++)
        {
            if (isspace(colon_ptr[i]) != 0)
            {
                printf ("Label error. Invalid characters after \':\'");
                return -1;
            }
        }
    }
    
    return (int) (colon_ptr - string);
}
