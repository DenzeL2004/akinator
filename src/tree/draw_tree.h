#ifndef _DRAW_TREE_H_
#define _DRAW_TREE_H_

#include "config_tree.h"

#include "tree.h"

static const int Max_command_buffer = 100;

static const int Mask_draw_node_modes = (1 << 4) - 1;

int Draw_tree_graph (const Tree *tree, const char *name_output_file, 
                          const int node_mode = Mask_draw_node_modes);

enum Draw_tree_func_err
{
    TREE_DRAW_GRAPH_ERR     = -1,
};

enum Tree_node_draw_modes
{
    DRAW_PTR        = (1 << 0),
    DRAW_DATA       = (1 << 1),

    DRAW_SONS_PTR   = (1 << 2),

    DRAW_ID    = (1 << 3),
};

#endif  //#endif _DRAW_TREE_H_