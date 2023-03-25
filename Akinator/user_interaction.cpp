#include "Akinator.h"

void greeting ()
{
    printf ("Meow! Welcome to the Akinator game!\n");
}


void goodbye ()
{
    printf ("Thank you for the game! See you later\n");
}

void game_modes_enumeration ()
{
    printf ("%d.Akinator\n"
            "%d.See the picture of akinator base\n"
            "%d.Description mode\n", 
            AKINATOR_MODE, 
            DUMP_MODE,
            DESCRIPTION_MODE);
}
