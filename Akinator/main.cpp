#include "Akinator.h"

int main ()
{
    greeting();
    
    //struct Node* node = create_node ("Неизвестно кто", strlen("Неизвестно кто") + 1);
    //printf ("Вы загадали %s?\n", node -> data);
    /*struct Node left;
    left.data = strdup ("left");
    struct Node right;
    right.data = strdup ("right");
    node.left = &left;
    node.right = &right;
    */
    struct Node* node = read_source ();
    //printf ("%s\n", node -> data);
    //printf ("%s\n", node -> left -> data);
    //printf ("%s\n", node -> right -> data);
    game_mode (node);
    
    DFS_free (node);
    goodbye();
    
    return 0;
}
