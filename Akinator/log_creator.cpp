#include "Akinator.h"

static int DFS_init (const struct Node* node, size_t* counter, FILE* graph_file)
{
    if (!node) return 0;
    fprintf (graph_file, "\tnode%lu [label = \"%s\"];\n", *counter, node -> data);
    
    *counter += 1;
    DFS_init (node -> left, counter, graph_file);
    *counter += 1;
    DFS_init (node -> right, counter, graph_file);
    //*counter -= 2;
    
    return 0;
}

static int DFS_connection (const struct Node* node, size_t* counter, FILE* graph_file)
{
    //printf ("counter = %d node = %x\n", *counter, node);
    if (!node) return 0;
    //printf ("data = %d\n", node -> data);
    
    size_t cur_counter = *counter;
    
    if (node -> left != NULL)
        fprintf (graph_file, "\tnode%lu -> node%lu;\n", cur_counter, *counter + 1);
    *counter += 1;
    DFS_connection (node -> left, counter, graph_file);
    
    if (node -> right != NULL)
        fprintf (graph_file, "\tnode%lu -> node%lu;\n", cur_counter, *counter + 1);
    *counter += 1;
    DFS_connection (node -> right, counter, graph_file);
    
    //*counter -= 2;
    
    return 0;
}

int create_picture (const struct Node* node)
{
    assert (node || "node ptr in create_picture function is null\n");
    
    const int COMPILE_STR_SIZE = 50;
    char compile_string[COMPILE_STR_SIZE];
    sprintf (compile_string, "dot -Tpng graph.txt -o graph.png");//, dump_counter);
    FILE* graph_file = fopen ("graph.txt", "w");
    
    fprintf (graph_file, 
             "digraph G\n"
             "{\n"
                "\tgraph [dpi = 100];\n"
                "\tranksep = 1.5;\n"
                "\tsplines = ortho\n\n"

                "\tedge[minlen = 3, penwidth = 3];\n"
                "\tnode[shape = record, style = rounded,\n"
                    "\t\tfixedsize = true, height = 1, width = 5,\n"
                    "\t\tfontsize = 30];\n\n");
    
    size_t counter = 0;
    DFS_init (node, &counter, graph_file);
    counter = 0;
    fprintf (graph_file, "\n");
    DFS_connection (node, &counter, graph_file);
    
    fprintf (graph_file, 
             "}\n");
    
    fclose (graph_file);
    system (compile_string);
    
    return 0;
}

int save_progress (struct Node* node, int ret_val)
{
    char buffer[MAX_BUFF_SIZE + 1];
    
    if (ret_val == AKINATOR_MODE)
    {
        printf ("Save progress?\n");
        while (true)
        {
            myfgets (buffer, MAX_BUFF_SIZE, stdin);
            if (strncmp(buffer, "yes", 3) == 0)
            {
                create_tree_note (node);
                return 0;
            }
            else if (strncmp(buffer, "no", 2) == 0)
            {
                return 0;
            }
            else
            {
                printf ("Enter yes or no\n");
            }
        }
    }
    
    return 0;
}

int create_tree_note (struct Node* node)
{
    FILE* note = fopen ("tree_structure.txt", "w");
    
    DFS_record (node, note);
    
    fclose (note);
    
    return 0;
}

int DFS_record (struct Node* node, FILE* where_to_write)
{
    if (!node) return 0;
    fprintf (where_to_write, "{ \"%s\" ", node -> data);
    
    if (node -> left == NULL && node -> right == NULL)
    {
        fprintf (where_to_write, "}\n");
        
        return 0;
    }
    
    fputc ('\n', where_to_write);
    
    if (node -> left != NULL)
    {
        DFS_record (node -> left, where_to_write);
    }
    
    if (node -> right != NULL)
    {
        DFS_record (node -> right, where_to_write);
    }
    
    fprintf (where_to_write, "} \n");
    
    return 0;
}

struct Node* read_source ()
{
    FILE* stream = fopen ("tree_structure.txt", "r");
    
    struct Node* node = NULL;
    if (stream == NULL)
    {
        node = create_node ("Unknown who", strlen("Unknown who") + 1);
    }
    else
    {
        DFS_read (&node, stream);
        fclose (stream);
    }
    
    
    return node;
}

int DFS_read (struct Node** node, FILE* stream)
{
    //if (!node) return 0;
    char buffer[MAX_BUFF_SIZE + 1] = {0};
    fscanf (stream, "%s ", buffer);
    if (buffer[0] == '{')
    {
        *node = (struct Node*) malloc (sizeof (struct Node));
    }
    else
    {
        printf ("The description of the tree does not follow the rules\n");
        return -1;
    }
    
    fscanf (stream, "\"%[^\"]", buffer);
    size_t elems_num = strlen (buffer) + 1;
    //printf ("%s\n", buffer);
    (*node) -> data = (char*) calloc (elems_num, sizeof (char));
    strncpy ((*node) -> data, buffer, elems_num);
    memset ((void*) buffer, 0, MAX_BUFF_SIZE + 1);
    
    //printf ("%s\n", node -> data);
    fgetc (stream);
    fscanf (stream, "%c%c", buffer, buffer + 1);
    
    //printf ("%s\n", buffer);
    if (buffer[1] == '}')
    {
        (*node) -> left = NULL;
        (*node) -> right = NULL;
        return 0;
    }
    else if (buffer[1] == '\n')
    {
        DFS_read (&((*node) -> left), stream);
        DFS_read (&((*node) -> right), stream);
    }
    
    fscanf (stream, "%c%c", buffer, buffer + 1);
    //printf ("%s\n", buffer);
    
    return 0;
}
