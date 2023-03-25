#include "Akinator.h"

int game_mode (struct Node* node)
{
    printf ("What mode would you like to play in?\n");
    game_modes_enumeration ();
    
    int choice = fgetc (stdin) - '0';
    //printf ("your choice: %d\n", choice);
    
    int ret_val = choice;
    if (choice == AKINATOR_MODE)
    {
        while (ret_val == AKINATOR_MODE)
        {
            ret_val = akinator (node);
        }
        ret_val = AKINATOR_MODE;
    }
    if (choice == DUMP_MODE)
    {
        create_picture (node);
        system ("xdg-open graph.png");
    }
    
    save_progress (node, ret_val);
    
    return 0;
}

int akinator (struct Node* node)
{
    char buffer[MAX_BUFF_SIZE + 1];
 
    while (true)
    {
        if (node -> left == NULL && node -> right == NULL)
        {
            printf ("Was your word a %s?\n", node -> data);
            myfgets (buffer, MAX_BUFF_SIZE, stdin);
            //printf ("buffer: %s\n", buffer);
            if (strncmp(buffer, "yes", 3) == 0)
            {
                printf ("It was easy to me\n");
                
                printf ("To play again, enter 0. To end the game, any other character\n");
                myfgets (buffer, MAX_BUFF_SIZE, stdin);
                
                if (strcmp (buffer, "0") == 0)
                    return AKINATOR_MODE;
                
                return -1;
            }
            else if (strncmp(buffer, "no", 2) == 0)
            {
                printf ("You beat me this time... What did you wish for?\n");
                myfgets (buffer, MAX_BUFF_SIZE, stdin);
                node -> left  = create_node (buffer, strlen(buffer) + 1);
                node -> right = create_node (node -> data, strlen(node -> data) + 1);
                printf ("How does  %s differ from %s?\nIt is... ", buffer, node -> data);
                myfgets (buffer, MAX_BUFF_SIZE, stdin);
                free (node -> data);
                node -> data = strdup (buffer);
                printf ("Thanks for the information. Take care, man\n");
                
                printf ("To play again, enter 0. To end the game, any other character\n");
                myfgets (buffer, MAX_BUFF_SIZE, stdin);
                
                if (strcmp (buffer, "0") == 0)
                    return AKINATOR_MODE;
                
                return -1;
            }
            else
            {
                printf ("Enter yes or no\n");
            }
        }
        else
        {
            printf ("Is it a %s?\n", node -> data);
            myfgets (buffer, MAX_BUFF_SIZE, stdin);
            if (strncmp(buffer, "yes", 3) == 0)
            {
                node = node -> left;
            }
            else if (strncmp(buffer, "no", 2) == 0)
            {
                node = node -> right;
            }
            else
            {
                printf ("Enter yes or no\n");
            }
        }
    }
}

int description_mode (struct Node* node)
{
    char buffer[MAX_BUFF_SIZE + 1];
    printf ("Enter the word:\n");
    myfgets (buffer, MAX_BUFF_SIZE, stdin);
    
    Stack stk = {};
    stack_ctor (&stk, 5);
    
    DFS_search (node, &stk, buffer);
    
    stack_dtor (&stk);
    
    return 0;
}

int DFS_search (struct Node* node, Stack* stk, char* buffer)
{
    if (!node) return 0;
    
    struct Descrip trash;
    if (strcmp(node -> data, buffer) == 0)
    {
        printf ("%s - ", buffer);
        while (stk -> size > 0)
        {
            stack_pop (stk, &trash);
            if (trash.point)
            {
                printf ("not ");
            }
            printf ("%s;", trash.node -> data);
        }
        
        return 0;
    }
    struct Descrip node_description;
    node_description.node = node;
    
    if (node -> left != NULL)
    {
        node_description.point = false;
        stack_push (stk, node_description);
        
        DFS_search (node -> left, stk, buffer);
        
        stack_pop (stk, &trash);
    }
    
    if (node -> right != NULL)
    {
        node_description.point = true;
        stack_push (stk, node_description);
        
        DFS_search (node -> right, stk, buffer);
        
        stack_pop (stk, &trash);
    }
    
    
    return 0;
}
