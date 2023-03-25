#include <stdio.h>
#include <string.h>

int main ()
{
    char buff[1000] = {0};
    scanf ("\"%[^\"]", buff);
    
    printf ("%s", buff);
}
