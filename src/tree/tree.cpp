#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#include "tree.h"
#include "draw_tree.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"
#include "../process_text/process_text.h"


/** 
 * @brief Creates an empty tree node
 * @return Returns a pointer to the node if the node was created, otherwise returns nullptr
*/
static Node* Creat_node ();

/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
static int Delete_node (Node **node);


static int Save_node_to_file (const Node *node, FILE *fpout);

static int Read_node_from_buffer (Node *node, Text_info *text);


static uint64_t Check_tree (const Tree *tree);


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

    tree->root = Creat_node ();

    if (Check_nullptr (tree->root))
    {
        Log_report ("Memory allocation error, root = |%p|\n", 
                                             (char*) tree->root);
        Err_report ();

        return TREE_CTOR_ERR;
    }

    tree->buffer_ptr = nullptr;

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

    Removing_subtree_from_node (&tree->root);
    
    tree->cnt_nodes = -1;

    free (tree->buffer_ptr);

    return 0;
}

//======================================================================================

void Removing_subtree_from_node (Node **node)
{
    assert ( node != nullptr && " node is nullptr");
    assert (*node != nullptr && "*node is nullptr");

    if (!Check_nullptr ((*node)->left))    Removing_subtree_from_node (&(*node)->left);
    if (!Check_nullptr ((*node)->right))   Removing_subtree_from_node (&(*node)->right);

    if (Delete_node (node))
    {
        Log_report ("Failed to delete node by pointer = |%p|\n", (char*) (*node));
        Err_report ();
        return;
    }

    return;
}

//======================================================================================

Node* Creat_node ()
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
    assert ( node != nullptr && " node is nullptr");
    assert (*node != nullptr && "*node is nullptr");

    (*node)->left  = nullptr;
    (*node)->right = nullptr;

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

    Node *new_node = Creat_node ();
    if (Check_nullptr (new_node))
    {
        Log_report ("Memory allocation error");
        Err_report ();
        return ERR_MEMORY_ALLOC;
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

int Save_tree_to_file (const Tree *tree, const char *name_output_file)
{
    assert (tree != nullptr && "tree is nullptr");

    FILE *fpout = Open_file_ptr (name_output_file, "w");

    if (Check_nullptr (fpout)) 
    {
        Log_report ("Error opening output file named \"%s\"\n", name_output_file);
        Err_report ();
        return ERR_FILE_OPEN;
    }

    if (Save_node_to_file (tree->root, fpout))
    {
        Log_report ("Error saving a tree with a pointer to the root |%p|\n", (char*) tree->root);
        Err_report ();
        return SAVING_DATABASE_ERR;
    }

    if (Close_file_ptr (fpout)) 
    {
        Log_report ("Error close output file named \"%s\"\n", name_output_file);
        Err_report ();
        return ERR_FILE_CLOSE;
    }

    return 0;
}

//======================================================================================

static int Save_node_to_file (const Node *node, FILE *fpout)
{
    assert (node != nullptr && "node is nullptr");
    assert (fpout != nullptr && "fpout is nullptr");

    if (Is_leaf_node (node))
        fprintf (fpout, " { %" ELEM_T_SPEC " } ", node->data);

    else
    {
        fprintf(fpout, " { %" ELEM_T_SPEC, node->data);

        if (!Check_nullptr (node->left)) 
        {
            if (Save_node_to_file(node->left, fpout))
                return SAVING_NODE_ERR;
        }

        if (!Check_nullptr (node->right)) 
        {
            if (Save_node_to_file(node->right, fpout))
                return SAVING_NODE_ERR;
        }

        fprintf(fpout, " } ");
    }

    return 0;
}

//======================================================================================

int Read_tree_from_file (Tree *tree, const char *name_input_file)
{
    assert (tree != nullptr && "node is nullptr");
    assert (name_input_file != nullptr && "name_input_file is nullptr");

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
    {
        Log_report ("Error opening iputfile file named \"%s\"\n", name_input_file);
        Err_report ();
        return ERR_FILE_OPEN;
    }

    Text_info text = {};
    
    if (Text_read (fdin, &text))
    {
        Log_report ("Error reading into Text_info structure\n");
        Err_report ();
        return ERR_FILE_READING;
    }
    
    text.pos = 0;

    if (Read_node_from_buffer (tree->root, &text))
    {   
        Log_report ("Error read a tree with a pointer to the root |%p|\n", (char*) tree->root);
        Err_report ();
        return READ_DATABASE_ERR;
    }

    if (Close_file_discriptor (fdin))
    {
        Log_report ("Error close input file named \"%s\"\n", name_input_file);
        Err_report ();
        return ERR_FILE_CLOSE;
    }

    return 0;
}

//======================================================================================

static int Read_node_from_buffer (Node *node, Text_info *text)
{
    assert (text != nullptr && "text is nullptr");

    int shift = 0;
    char symbol = 0;
    
    int result_scanned = sscanf(text->text_buf + text->pos, " %c %n", &symbol, &shift);
    if (result_scanned != 1) 
    {
        Log_report ("Read error, was read: %d\n", result_scanned);
        Err_report ();
        return READ_NODE_ERR;
    }
    
    if (symbol != '{') 
    {
        Log_report ("Read error, undefined symbol, was read: |%c|\n", symbol);
        Err_report ();
        return READ_DATABASE_ERR;
    }

    text->pos += shift;
    elem_t *node_data = (elem_t*) (text->text_buf + text->pos);

    symbol = 0;
    int len_word = 0;

    while (symbol != '{' && symbol != '}') 
    {
        result_scanned = sscanf(text->text_buf + text->pos, "%*" ELEM_T_SPEC "%n %n%c", 
                                                    &len_word, &shift, &symbol);
        if (result_scanned != 1) 
        {
            Log_report ("Read error, was read: %d\n", result_scanned);
            Err_report ();
            return READ_NODE_ERR;
        }
        
        text->pos += shift;
    }
    
    *(text->text_buf + text->pos - shift + len_word) = '\0';
    
    node->data = node_data;

    if (*(text->text_buf + text->pos) == '}') 
    {
        text->pos++;
        return 0;
    }

    if (*(text->text_buf + text->pos) == '{') {
        
        node->left  = Creat_node ();
        if (Check_nullptr (node->left))
        {
            Log_report ("Memory allocation error");
            Err_report ();
            return ERR_MEMORY_ALLOC;
        }

        node->right  = Creat_node ();
        if (Check_nullptr (node->right))
        {
            Log_report ("Memory allocation error");
            Err_report ();
            return ERR_MEMORY_ALLOC;
        }


        if (Read_node_from_buffer(node->left, text))
        {
            Log_report ("Error reading the value of the left son = |%p|",
                                                                (char*) node->left);
            Err_report ();
            return READ_NODE_ERR;
        }

        if (Read_node_from_buffer(node->right, text))
        {
            Log_report ("Error reading the value of the left son = |%p|",
                                                                (char*) node->right);
            Err_report ();
            return READ_NODE_ERR;
        }


        result_scanned = sscanf(text->text_buf + text->pos, " %c %n", &symbol, &shift);
        if (result_scanned != 1)
        {
            Log_report ("Read error, was read: %d\n", result_scanned);
            Err_report ();
            return READ_NODE_ERR;
        }
        

        if (symbol == '}') 
        {
            text->pos += shift;
            return 0;
        }

        else 
        {
            Log_report ("Read error, undefined symbol, was read: |%c|\n", symbol);
            Err_report ();
            return READ_NODE_ERR;
        } 

    }
    
    return READ_NODE_ERR;
}

//======================================================================================

int Tree_dump_ (const Tree *tree,
                const char* file_name, const char* func_name, int line, const char *format, ...)
{
    assert (tree != nullptr && "tree is nullptr");

    uint64_t err = Check_tree (tree);

    FILE *fp_logs = Get_log_file_ptr ();

    fprintf (fp_logs, "=================================================\n\n");

    va_list args = {};

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

        static int Cnt_graphs = 0;      //<-To display the current tree view

        char output_img[Max_command_buffer] = {0};
        sprintf (output_img, "graph_img/picture%d.png", Cnt_graphs); 

        Draw_tree_graph (tree, output_img);

        Cnt_graphs++;
    
    #endif

    fprintf (fp_logs, "\n");

    fprintf (fp_logs, "==========================================================\n\n");

    return 0;
}

//======================================================================================

static void Print_tree_variables (const Tree *tree, FILE *fpout)
{
    assert (tree  != nullptr &&  "tree is nullptr");
    assert (fpout != nullptr && "fpout is nullptr");

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


    if (Check_nullptr (tree->root)) err |= ROOT_IS_NULLPTR;

    return err;
}

//======================================================================================