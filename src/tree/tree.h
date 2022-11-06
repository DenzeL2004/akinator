#ifndef _TREE_H_
#define _TREE_H_



#include "config_tree.h"
#include "../log_info/log_def.h"


const int Max_definition_buffer = 100;

const int Max_object_buffer     = 100;

static const char *Default_output_file_name = "last_save_base.txt";


struct Node
{
    elem_t *data = nullptr;
    Node   *right = nullptr;
    Node   *left  = nullptr;
};

struct Tree
{
    long cnt_nodes      = 0;

    Node *root = nullptr;

    char *buffer_ptr    = nullptr;
};


enum Tree_func_err
{
    TREE_CTOR_ERR           = -1,
    TREE_DTOR_ERR           = -2,

    SAVING_NODE_ERR         = -3,
    SAVING_DATABASE_ERR     = -4,

    READ_NODE_ERR           = -5,
    READ_DATABASE_ERR       = -6,

    TREE_ADD_NODE_ERR       = -7,
};

enum Tree_err
{
    NEGATIVE_CNT                = (1 << 0),

    ROOT_IS_NULLPTR             = (1 << 1),
    
};


int Tree_ctor (Tree *tree);

int Tree_dtor (Tree *tree);


/** 
 * @brief Adds a node to the tree
 * @version 1.0.0
 * @param [in] tree Structure Tree pointer
 * @param [in] node_ptr Pointer where the pointer of the new node will be stored
 * @return Processing of the add command was correct, otherwise returns a non-zero number
*/
int Add_node_to_pointer (Tree *tree, Node **ptr);


/** 
 * @brief Removes all links and node values from the subtree
 * @param [in] node Pointer to Node pointer
*/
void Removing_subtree_from_node (Node **node);


int Save_tree_to_file (const Tree *tree, const char *name_output_file = Default_output_file_name);

int Read_tree_from_file (Tree *tree, const char *name_input_file);


int Is_leaf_node (const Node *node);


#define Tree_dump(tree, ...)                       \
        Tree_dump_ (tree, LOG_ARGS, __VA_ARGS__)

int Tree_dump_ (const Tree *tree, LOG_PARAMETS, const char *format, ...);

#endif  //#endif _TREE_H_