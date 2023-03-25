#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "asm.h"

size_t strlen_my (char* s);
int fputs_my (const char *s, FILE* stream);
int strcmp_my_reversed (const char* str1, size_t str1_size, const char* str2, size_t str2_size);
int strcmp_my (const char* string1, const char* string2);

size_t nstring_counter (const char* array, size_t size)
{
    assert (array);
    
    size_t counter = 0;
    if (*(array + size - 1) != '\n')
        counter++;
    
    while (*array)
    {
        if (*array == '\n')
        {
            while (*(array + 1) == '\r' || *(array + 1) == '\n') //< skip empty strings 
                array++;
            
            counter++;
        }
        
        array++;
    }
    
    return counter;
}
//int array_p_make (struct str_pointer* *array_of_pointers, char* array_text, size_t text_size, size_t nstrings);
int array_p_make (struct str_pointer* *array_of_pointers, char* array_text, size_t nstrings)
{
    assert (array_text);
    assert (*array_of_pointers == NULL && "Error: array_of_pointers must be a NULL pointer\n");
    
    struct str_pointer* array_p = (struct str_pointer*) calloc (nstrings + 1, sizeof (struct str_pointer));
    if (array_p == NULL)
    {
        printf ("Error: I can't create array of pointers\n");
        return -1;
    }
    
    size_t counter = 0;
    
    array_p[counter].p = array_text;
    counter++;
    while ((counter < nstrings) && (*array_text))
    {
        if (*array_text == '\n')
        {
            *array_text = '\0';
            while (*(array_text + 1) == '\r' || *(array_text + 1) == '\n') //< skip empty strings 
                array_text++;
            
            array_p[counter].p = array_text + 1;
            counter++;
        }
        
        array_text++;
    }
    
    for (counter = 0; counter < nstrings - 1; counter++)
    {
        array_p[counter].size = (size_t) (array_p[counter + 1].p - array_p[counter].p);
    }
    
    array_p[counter].size = strlen_my (array_p[counter].p);
    
    *array_of_pointers = array_p;
    
    return 0;
}

int strcmp_my (const char* string1, const char* string2)
{
    assert (string1);
    assert (string2);
    
    int difference = 0;
    while (*string1 != '\n' && *string2 != '\n' && *string1 && *string2)
    {
        if (isalnum (*string1) && isalnum (*string2))
        {
            difference = *string1 - *string2;
            if (difference != 0)
                return difference;
            string1++;
            string2++;
        }
        else if (!isalnum (*string1))
        {
            string1++;
        }
        else
        {
            string2++;
        }
    }
    
    return *string1 - *string2;
}

int strcmp_my_reversed (const char* str1, size_t str1_size, const char* str2, size_t str2_size)
{
    assert (str1);
    assert (str2);
    
    const char* string1 = str1 + str1_size - 1;
    const char* string2 = str2 + str2_size - 1;

    int difference = 0;
    while ((str1_size-- > 0) && (str2_size-- > 0))
    {
        if (isalpha (*string1) && isalpha (*string2))
        {
            difference = *string1 - *string2;
            if (difference != 0)
                return difference;
            string1--;
            string2--;
        }
        else if (!isalpha (*string1))
        {
            string1--;
        }
        else
        {
            string2--;
        }
    }
    
    return *string1 - *string2;
}

int fputs_my (const char *s, FILE* stream)
{
    assert (s);
    assert (stream);
    
    while (*s != '\n' && *s) 
    {
        if (fputc (*s++, stream) == EOF)
            return EOF;
    }
    
    return fputc ('\n', stream);
}

size_t strlen_my (char* s)
{
    assert (s);
    
    size_t counter = 0;
    while (*s != '\n' && *s)
    {
        counter++;
        s++;
    }
    counter++;
    return counter;
}

int stricmp (const char* string1, const char* string2)
{
    char ch1 = *string2;
    char ch2 = *string1;
    
    for (int i = 1; ch1 && ch2; i++)
    {
        if (tolower(ch1) - tolower(ch2) != 0)
            return tolower(ch1) - tolower(ch2);
        
        ch1 = *(string2 + i);
        ch2 = *(string1 + i);
    }
    
    return tolower(ch1) - tolower(ch2);
}
