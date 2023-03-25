#ifndef AKINATOR_HEADER
#define AKINATOR_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define DATA_FORMAT "%s"

struct Node
{
    char* data = 0;
    struct Node* left = NULL;
    struct Node* right = NULL;
};

struct Descrip
{
    struct Node* node = NULL;
    bool point = false;
};
#include "stack.h"

const int MAX_BUFF_SIZE = 100;

//Game modes constants
const int AKINATOR_MODE = 0;
const int DUMP_MODE = 1;
const int DESCRIPTION_MODE = 2;

//logs
int create_picture (const struct Node* node);
int create_tree_note (struct Node* node);
int DFS_record (struct Node* node, FILE* where_to_write);
struct Node* read_source ();
int DFS_read (struct Node** node, FILE* stream);

//tree functions
struct Node* create_node (const char* data, size_t elems_num);
char* myfgets (char* s, int size, FILE* stream);
int DFS_free (struct Node* node);

//Game modes
int game_mode (struct Node* node);
int akinator (struct Node* node);
int save_progress (struct Node* node, int ret_val);
int description_mode (struct Node* node);
int DFS_search (struct Node* node, Stack* stk, char* buffer);

//User interaction
void greeting ();
void goodbye ();
void game_modes_enumeration ();

#endif
