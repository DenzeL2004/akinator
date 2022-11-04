#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "draw_tree.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"



static void Draw_nodes (FILE *fpout, const Node *node, int *counter);



//======================================================================================

int Draw_tree_graph (const Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr\n");

    FILE *graph = Open_file_ptr ("temp/graph_img/graph.txt", "w");
    if (Check_nullptr (graph))
    {
        Err_report ();
        return TREE_DRAW_GRAPH_ERR;
    }

    fprintf (graph, "digraph G{\n");
    fprintf (graph, "rankdir=TB;\n");
    fprintf (graph, "splines=spline;\n");
    fprintf (graph, "{\n");

    int counter = 0;
    Draw_nodes (graph, tree->root, &counter);


    fprintf(graph, "}\n}\n");
    fclose(graph);

    
    static int Cnt_graphs = 0;      //<-To display the current tree view

    char command_buffer[Max_command_buffer] = {0};
    sprintf(command_buffer, "dot -Tpng temp/graph_img/graph.txt"
                            " -o temp/graph_img/picture%d.png", Cnt_graphs);

    if (system(command_buffer))
    {
        Err_report ();
        return TREE_DRAW_GRAPH_ERR;
    }

    FILE *fp_logs = Get_log_file_ptr ();
    if (Check_nullptr (fp_logs))
    {
        Err_report ();
        return TREE_DRAW_GRAPH_ERR;
    }

    fprintf (fp_logs, "<img src = graph_img/picture%d.png />\n", Cnt_graphs);
                                
    Cnt_graphs++;
    return 0;
}

//======================================================================================

static void Draw_nodes (FILE *fpout, const Node *node, int *counter)
{
    assert (node != nullptr && "node is nullptr\n");

    (*counter)++;
    
    

    char* ch_right_node_ptr = (char*) node->right;
    char* ch_left_node_ptr  = (char*) node->left;

    char *ch_ptr = (char*) node;

    fprintf (fpout, "node%p [style=filled, shape = record, label =  \"{ID: %d |NODE POINTER: %p | DATA: %"ELEM_T_SPEC" | left: %p | right: %p}\",", 
                        ch_ptr, *counter, ch_ptr, node->data, ch_left_node_ptr, ch_right_node_ptr);

    if (Is_leaf_node (node))
        fprintf (fpout, " fillcolor=lightgreen ];\n");
    else
        fprintf (fpout, " fillcolor=lightgoldenrod1 ];\n");


    if (!Check_nullptr (node->left)) 
    {
        Draw_nodes (fpout, node->left,  counter);

        fprintf (fpout, "node%p -> node%p[style=filled, color=royalblue3];\n", 
                                ch_ptr, ch_left_node_ptr);    
    }

    if (!Check_nullptr (node->right))
    {
        Draw_nodes (fpout, node->right, counter);

        fprintf (fpout, "node%p -> node%p[style=filled, color=red3];\n", 
                                ch_ptr, ch_right_node_ptr);
    }

   

    return;
}

//======================================================================================