#include <stdio.h>
#include <string.h>
#include <fcntl.h> /*read/write flags*/
#include <sys/types.h> /*types definitions*/
#include <sys/stat.h> /*the struct which stat returns*/
#include "asm.h"
#include <assert.h>

size_t fsize (char* name)
{
    assert (name);
    
    struct stat stbuf;
    if (stat (name, &stbuf) == -1)
    {
        printf ("Error: can't get information about %s file\n", name);
        return 0;
    }
    if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
    {
        printf ("Error: this is the directory!\n");
        return 0;
    }
    
    return (size_t) stbuf.st_size;
}
