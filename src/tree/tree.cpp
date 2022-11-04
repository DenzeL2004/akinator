#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "draw_tree.h"

#include "tree.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"


/** 
 * @brief Creates an empty tree node
 * @return Returns a pointer to the node if the node was created, otherwise returns nullptr
*/
static Node* Create_node ();

/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
static int Delete_node (Node **node);

/** 
 * @brief Removes all links and node values from the subtree
 * @param [in] node Pointer to Node pointer
*/
void Removing_subtree_from_root (Node **node);



static uint64_t Check_tree (const Tree *tree);

int Is_leaf_node (const Node *node);


static void Print_tree_variables (const Tree *tree, FILE *fpout);

static void Print_error_value (uint64_t err, FILE *fpout);



#define REPORT(...)                                         \
    {                                                       \
        Tree_dump (tree, __VA_ARGS__);                      \
        Err_report ();                                      \
                                                            \
    }while (0)
                                    
//======================================================================================

int Tree_ctor (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    tree->root = Create_node ();

    if (Check_nullptr (tree->root))
    {
        Log_report ("Memory allocation error, root = |%p|\n", 
                                             (char*) tree->root);
        Err_report ();

        return TREE_CTOR_ERR;
    }

    tree->cnt_nodes = 1;

    return 0;
}

//======================================================================================

int Tree_dtor (Tree *tree)
{
    assert (tree != nullptr && "tree ptr is nullptr");

    if (Check_tree (tree))
    {
        REPORT ("ENTRY\nFROM: Tree_dtor\n");
        return TREE_DTOR_ERR;
    }

    Removing_subtree_from_root (&tree->root);
    
    tree->cnt_nodes = -1;

    return 0;
}

//======================================================================================

void Removing_subtree_from_root (Node **node)
{
    assert ( node != nullptr && " node is nullptr\n");
    assert (*node != nullptr && "*node is nullptr\n");

    if (!Check_nullptr ((*node)->left))    Removing_subtree_from_root (&(*node)->left);
    if (!Check_nullptr ((*node)->right))   Removing_subtree_from_root (&(*node)->right);

    if (Delete_node (node))
    {
        Log_report ("Failed to delete node by pointer = |%p|\n", (char*) (*node));
        Err_report ();
        return;
    }

    return;
}

//======================================================================================

Node* Create_node ()
{
    Node *new_node = (Node*) calloc (1, sizeof (Node));
    if (Check_nullptr (new_node))
    {
        Log_report ("Memory allocation error, new_node = |%p|\n", 
                                             (char*) new_node);
        Err_report ();

        return nullptr;
    }

    new_node->left  = nullptr;
    new_node->right = nullptr;
    new_node->data  = nullptr;

    return new_node;
}

//======================================================================================

static int Delete_node (Node **node)
{
    assert ( node != nullptr && " node is nullptr\n");
    assert (*node != nullptr && "*node is nullptr\n");

    (*node)->left  = nullptr;
    (*node)->right = nullptr;

    free ((*node)->data);
    (*node)->data = nullptr;

    free ((*node));
    (*node) = nullptr;

    return 0;
}

//======================================================================================

int Add_node_to_pointer (Tree *tree, Node **ptr) 
{
    assert (tree != nullptr && "tree is nullptr");
    assert (ptr  != nullptr && "ptr is nullptr");

    if (Check_tree (tree))
    {
        REPORT ("ENTRY\nFROM: ", __PRETTY_FUNCTION__,
                        ", ptr = |%p|\n", (char*) *ptr);
        return TREE_ADD_NODE_ERR;
    }
    
    if (!Check_nullptr (*ptr))
    {
        Log_report ("Pointer is not nullptr, can't re-hang node\n");
        return TREE_ADD_NODE_ERR;
    }

    Node *new_node = Create_node ();
    if (Check_nullptr (new_node))
    {
        Log_report ("Memory allocation error, new_node = |%p|\n", 
                                             (char*) new_node);
        Err_report ();

        return TREE_ADD_NODE_ERR;
    }

    *ptr = new_node;

    tree->cnt_nodes++;

    if (Check_tree (tree))
    {
        REPORT ("EXIT\nFROM: ", __PRETTY_FUNCTION__,
                        ", ptr = |%p|\n", (char*) *ptr);

        return TREE_ADD_NODE_ERR;
    }

    return 0;
}

//======================================================================================

int Is_leaf_node (const Node *node)
{
    assert (node != nullptr && "node is nullptr");

    return (Check_nullptr (node->left) && Check_nullptr (node->right));
}

//======================================================================================

int Akinator (Tree *tree, Node *node)
{
    assert (tree != nullptr && "tree is nullptr\n");

    if (Check_nullptr (node->data))
    {
        printf ("I don't know who you asked me. Please, tell me, who it is?\n");

        char object[Max_object_buffer] = {0};
        if (scanf ("%s", object) != 1)
        {
            Log_report ("Error reading from console\n");
            Err_report ();
            return AKINATOR_GAME_ERR;
        }
        
        node->data = strdup (object);
        if (Check_nullptr (node->data))
        {
            Log_report ("Memory allocation error\n");
            Err_report ();
            return AKINATOR_GAME_ERR;
        }

        printf ("Thank you, now I know more, next time I'll guess right.\n");

        return 0;
    }
    
    if (Is_leaf_node (node))
    {
        printf ("It is %s?\n", node->data);
        printf ("Press Y - YES\nPress N - NO\n");

        Mu_flush ();

        if (getchar () == 'Y')
            printf ("Haha, I won!!!!\n");
        else
        {
            printf ("It seems I do not know who you thought of, who is it?\n");

            char object[Max_object_buffer] = {0};
            if (scanf ("%s", object) != 1)
            {
                Log_report ("Error reading from console\n");
                Err_report ();
                return AKINATOR_GAME_ERR;
            }

            printf ("I'm curious, what's the difference between them?\n");

            char definition[Max_definition_buffer] = {0};
            if (scanf ("%s", definition) != 1)
            {
                Log_report ("Error reading from console\n");
                Err_report ();
                return AKINATOR_GAME_ERR;
            }


            
            if (Add_node_to_pointer (tree, &node->left))
            {
                Log_report ("Error adding node by pointer = |%p|\n", (char*) node->left);
                Err_report ();
                return AKINATOR_GAME_ERR;
            }

            if (Add_node_to_pointer (tree, &node->right))
            {
                Log_report ("Error adding node by pointer = |%p|\n", (char*) node->right);
                Err_report ();
                return AKINATOR_GAME_ERR;
            }



            node->right->data = node->data;
            
            node-> left->data = strdup (object);
            if (Check_nullptr (node->left->data))
            {
                Log_report ("Memory allocation error\n");
                Err_report ();
                return AKINATOR_GAME_ERR;
            }

            node->data = strdup (definition);
            if (Check_nullptr (node->data))
            {
                Log_report ("Memory allocation error\n");
                Err_report ();
                return AKINATOR_GAME_ERR;
            }


            printf ("Thank you, now I know more, next time I'll guess right.\n");
        }

        return 0;
    }

    else
    {
        printf ("%s?\n", node->data);
        printf ("Press Y - YES\nPress N - NO\n");
        
        Mu_flush ();
        
        switch (getchar ())
        {
            case 'Y':
                Akinator (tree, node->left);
                break;

            case 'N':
                Akinator (tree, node->right);
                break;
            
            default:
                Log_report ("Incorrectly entered command\n");
                return AKINATOR_GAME_ERR;
        } 

        return 0;   
    }

    return AKINATOR_GAME_ERR;
}

//======================================================================================

int Tree_dump_ (const Tree *tree,
                const char* file_name, const char* func_name, int line, const char *format, ...)
{
    assert (tree != nullptr && "tree is nullptr\n");

    uint64_t err = Check_tree (tree);

    FILE *fp_logs = Get_log_file_ptr ();

    fprintf (fp_logs, "=================================================\n\n");

    va_list args = nullptr;

    va_start(args, format);

    fprintf (fp_logs, "<h2>");
    vfprintf(fp_logs, format, args);
    fprintf (fp_logs, "</h2>");
    
    va_end(args);

    fprintf (fp_logs, "REFERENCE:\n");

    if (err) 
        fprintf (fp_logs, "ERROR\nCaused an error in file %s, function %s, line %d\n\n", LOG_VAR);
    else
        fprintf (fp_logs, "OK\nlast call in file %s, function %s, line %d\n\n", LOG_VAR);
    
                     
    if (err)
    {
        fprintf (fp_logs, "ERR CODE: ");
        Bin_represent (fp_logs, err, sizeof (err));
        fprintf (fp_logs, "\n");
    }

    Print_error_value (err, fp_logs);

    Print_tree_variables (tree, fp_logs);

    fprintf (fp_logs, "\n\n");

    #ifdef GRAPH_DUMP

        Draw_tree_graph (tree);
    
    #endif

    fprintf (fp_logs, "\n");

    fprintf (fp_logs, "==========================================================\n\n");

    return 0;
}

//======================================================================================

static void Print_tree_variables (const Tree *tree, FILE *fpout)
{
    assert (tree  != nullptr &&  "tree is nullptr\n");
    assert (fpout != nullptr && "fpout is nullptr\n");

    fprintf (fpout, "<body>\n");
    fprintf (fpout, "<table border=\"1\">\n");
    
    fprintf (fpout, "<tr><td> pointer to root </td> <td> %p </td></tr>", (char*) tree->root);

    fprintf (fpout, "<tr><td> cnt nodes </td> <td>  %ld </td></tr>",  tree->cnt_nodes);

    fprintf (fpout, "</table>\n");
    fprintf (fpout, "</body>\n");
   
    return;
}

//======================================================================================

static void Print_error_value (uint64_t err, FILE *fpout)
{
    assert (fpout != nullptr && "fpout is nullptr\n");

    if (err & NEGATIVE_CNT)
        fprintf (fpout, "cnt_nodes is negative number\n");

    if (err & ROOT_IS_NULLPTR)
        fprintf (fpout, "root is nullptr\n");        

    fprintf (fpout, "\n\n");

    return;
}

//======================================================================================

static uint64_t Check_tree (const Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    uint64_t err = 0;

    if (tree->cnt_nodes < 0) err |= NEGATIVE_CNT;


    if (Check_nullptr (tree->root))           err |= ROOT_IS_NULLPTR;

    return err;
}

//======================================================================================