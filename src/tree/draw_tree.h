#ifndef _DRAW_TREE_H_
#define _DRAW_TREE_H_

#include "config_tree.h"

#include "tree.h"

const int Max_command_buffer = 100;

int Draw_tree_graph (const Tree *tree);

enum Draw_tree_func_err
{
    TREE_DRAW_GRAPH_ERR     = -1,
};

#endif  //#endif _DRAW_TREE_H_