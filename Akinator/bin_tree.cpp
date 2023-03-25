#include "Akinator.h"

struct Node* create_node (const char* data, size_t elems_num)
{
    struct Node* new_node = (struct Node*) malloc (sizeof (struct Node));
    new_node -> data = (char*) calloc (elems_num, sizeof (char));
    new_node -> left = NULL;
    new_node -> right = NULL;
    
    strncpy (new_node -> data, data, elems_num);
    
    return new_node;
}

char* myfgets (char* s, int size, FILE* stream)
{
    fgets (s, size, stream);
    int len = strlen (s);
    s[len - 1] = '\0';
    
    return s;
}

int DFS_free (struct Node* node)
{
    if (!node) return 0;
    
    if (node -> left != NULL)
    {
        DFS_free (node -> left);
    }
    
    if (node -> right != NULL)
    {
        DFS_free (node -> right);
    }
    
    free (node -> data);
    free (node);
    
    return 0;
}

//void unite_nodes (struct Node* host_node, struct Node* join_node)
