#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "windows.h"


#include "akinator_game.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"
#include "../src/stack/stack.h"



static int Parting_akinator ();

static int Print_modes ();

static int Greeting_akinator ();



static int Play_akinator (Tree *tree);

static int Akinator (Tree *tree, Node *node);

static int Print_database (Tree *tree);

static int Get_definition (Tree *tree);

static int Compare_objects (Tree *tree);

static int Find_object (Node *node, const char *name_obj, Stack *def);

//======================================================================================

#define DEF_MODE(name_mode, ...)                \
        if (!strcmpi (#name_mode, cur_cmd))     \
        {                                       \
            __VA_ARGS__                         \
        }                                       \
        else                                    \
                                                        

int Game (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    Greeting_akinator ();

    printf ("I have several modes of operation, what shall we do?\n");
    Print_modes ();

    while (true)
    {
        printf ("Enter command: ");

        char cur_cmd[Max_command_buffer] = {0};

        if (scanf ("%s", cur_cmd) != 1)
        {
            Log_report ("Error reading the operating mode of the akinator\n");
            Err_report ();
            return PLAY_GAME_ERR;
        }

        #include "akinator_modes.h"

        /*else*/
            Print_colour (RED, "I don't known this commands!\n");

        My_flush ();
    }

    Parting_akinator ();

    return 0;
}

#undef DEF_MODE

//======================================================================================

static int Greeting_akinator ()
{
    Print_colour (YELLOW, "Loading...\n");

    for (int time = 1; time <= 3; time++){
        Print_colour (YELLOW, "Connecting to the networ: %d%%\n", time * 33);
        Sleep (Delay_time);
    }

    Print_colour (GREEN, "\nComplete 100%%\n");

    printf ("\nHello, my name is Akinator Tom.\n");
    Print_colour (YELLOW, "I'm glad you started me up, let's play!!!\n");
    
    return 0;
}

//======================================================================================

static int Print_modes ()
{
    printf ("\n");

    printf ("1. Game. Akinator will try to guess your hidden word.\n");
    printf ("2. Print. A picture of a tree with all current nodes will be drawn.\n");
    printf ("3. Definition. Tom, using his database, will define the given word.\n");
    printf ("4. Сomparison. Tom, using his database, tries to compare objects.\n");
    printf ("5. Exit. Finish the game.\n");
    printf ("6. Modes. Print mode List.\n");

    printf ("\n");

    return 0;
} 

//======================================================================================

static int Parting_akinator ()
{
    Print_colour (YELLOW, "Have you already played it???\n");
    Print_colour (BLUE, "I will miss you, come back soon\n");

    return 0;
}

//======================================================================================

static int Print_database (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    if (Draw_tree_graph (tree, "graph_img/cur_graph.png"))
    {
        Log_report ("Error in graph drawing\n");
        Err_report ();
        return DRAW_DATABASE_ERR;
    }

    if (system ("@temp\\graph_img\\cur_graph.png"))
    {
        Log_report ("Failed to open image\n");
        Err_report ();
        return DRAW_DATABASE_ERR;
    }

    return 0;
}

//======================================================================================

static int Get_definition (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    printf ("Enter the name of the object you want to find\n");

    My_flush ();

    char name_obj[Max_command_buffer] = {0};
    if (scanf ("%s", name_obj) != 1)
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }


    Stack def = {};

    if (Stack_ctor (&def, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }


    if (Find_object (tree->root, name_obj, &def))
    {
        Print_colour (YELLOW, "Oh, I know what it is, listen.\n");
        
        int size = Stack_get_size (&def);
        Print_colour (GREEN, "%s ", name_obj);

        while (size > 0)
        {
            elem_t *node_data = nullptr;
            Stack_pop (&def, &node_data);

            Print_colour (GREEN, "%s ", node_data);
            size--;
        }

        printf ("\n");
    }

    else
    {
        Print_colour (BLUE, "You will be surprised, "
                            "but I don’t know what you thought of me, "
                            "it turned out awkwardly.\n");
    }


    if (Stack_dtor (&def))
    {
        Log_report ("Dtor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    return 0;
}

//======================================================================================

static int Compare_objects (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    printf ("Enter the names of the objects you want to compare\n");

    My_flush ();

    char name_obj1[Max_command_buffer] = {0};
    if (scanf ("%s", name_obj1) != 1)
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    Stack def_obj1 = {};

    if (Stack_ctor (&def_obj1, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    if (!Find_object (tree->root, name_obj1, &def_obj1))
    {
        Print_colour (BLUE, "\nI don't know of such %s.\n", name_obj1);
        return 0;
    }



    Stack def_obj2 = {};

    char name_obj2[Max_command_buffer] = {0};
    if (scanf ("%s", name_obj2) != 1)
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    if (Stack_ctor (&def_obj2, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    if (!Find_object (tree->root, name_obj2, &def_obj2))
    {
        Print_colour (BLUE, "\nI don't know of such %s.\n", name_obj2);
        return 0;
    }


    int size_def1 = Stack_get_size (&def_obj1);
    int size_def2 = Stack_get_size (&def_obj2);

    Print_colour (GREEN, "%s and %s similar in that neither are both ", 
                                                  name_obj1, name_obj2);

    while (size_def1 > 0 && size_def2 > 0)
    {
        elem_t *node_data1 = nullptr;
        elem_t *node_data2 = nullptr;

        Stack_pop (&def_obj1, &node_data1);
        Stack_pop (&def_obj2, &node_data2);

        if (!strcmp (node_data1, node_data2))
        {
            if (strcmp ("isn't", node_data1) && strcmp ("is", node_data1))
                printf (GREEN, "%s ", node_data1);
        }    

        else
        {
            Stack_push (&def_obj1, node_data1);
            Stack_push (&def_obj2, node_data2); 
            break;
        }
    
        size_def1--;
        size_def2--;
    }

    if (size_def1 > 0 || size_def2 > 0)
        Print_colour (RED, ", but ");

    if (size_def1 > 0){
        Print_colour (RED, "%s ", name_obj1);

        while (size_def1 > 0)
        {
            elem_t *node_data = nullptr;
            Stack_pop (&def_obj1, &node_data);

            Print_colour (RED, " %s", node_data);
            if (strcmp ("isn't", node_data) && strcmp ("is", node_data))
                Print_colour (RED, ", ");

            size_def1--;
        }
    }


    if (size_def2 > 0){
        Print_colour (RED, "%s", name_obj2);

        while (size_def2 > 0)
        {
            elem_t *node_data = nullptr;
            Stack_pop (&def_obj2, &node_data);

            Print_colour (RED, " %s", node_data);
            if (strcmp ("isn't", node_data) && strcmp ("is", node_data))
                Print_colour (RED, ", ");

            size_def2--;
        }
    }

    printf ("\n");


    if (Stack_dtor (&def_obj1))
    {
        Log_report ("Dtor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    if (Stack_dtor (&def_obj2))
    {
        Log_report ("Dtor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    return 0;
}

//======================================================================================

static int Find_object (Node *node, const char *name_obj, Stack *def)
{
    assert (node != nullptr && "node is nullptr");
    assert (def  != nullptr && "stack def is nullptr");
    assert (name_obj != nullptr && "name_obj is nullptr");

    
    if (!strcmpi (name_obj, node->data))    
        return 1;


    if (!Check_nullptr (node->left))
        if (Find_object (node->left, name_obj, def))
        {
            Stack_push (def, node->data);
            Stack_push (def, "is");
            return 1;
        }

    if (!Check_nullptr (node->right))
        if (Find_object (node->right, name_obj, def))
        {
            Stack_push (def, node->data);
            Stack_push (def, "isn't");
            return 1;
        }

    return 0;
}

static int Play_akinator (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    printf ("Let's start the game\n");

    bool let_play = true;

    while (let_play)
    {
        putchar ('\n');

        if (Akinator (tree, tree->root))
        {
            Log_report ("ERROR: Akinator function worked incorrectly\n");
            Err_report ();
            Tree_dump (tree, "PLAY AKINATOR");

            return PLAY_AKINATOR_ERR;
        }


        printf ("\n");
        printf ("Want to play again?\n");
        printf ("Press Y - YES\nPress N - NO\n");

        My_flush ();

        switch (getchar ())
        {
            case 'Y':
                let_play = true;
                break;
            
            case 'N':
                let_play = false;
                break;

            
            default:
                printf ("Incorrect command\n\n");
                return PLAY_AKINATOR_ERR;
        }
    } 
    
    return 0;
}

//======================================================================================

static int Akinator (Tree *tree, Node *node)
{
    assert (tree != nullptr && "tree is nullptr");

    if (Check_nullptr (node->data))
    {
        Print_colour (BLUE, "I don't know who you asked me. Please, tell me, who is it?\n");

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

        Print_colour (YELLOW, "Thank you, now I know more, next time I'll guess right.\n");

        return 0;
    }

    
    if (Is_leaf_node (node))
    {
        printf ("It is %s?\n", node->data);
        printf ("Press Y - YES\nPress N - NO\n");

        My_flush ();

        switch (getchar ())
        {
            case 'Y':
                Print_colour (YELLOW, "Haha, I won!!!!\n");
                break;

            case 'N':
            {
                Print_colour (BLUE, "It seems I don't know who you thought of, who is it?\n");

                My_flush ();

                char object[Max_object_buffer] = {0};
                if (scanf ("%s", object) != 1)
                {
                    Log_report ("Error reading from console\n");
                    Err_report ();
                    return AKINATOR_GAME_ERR;
                }

                printf ("I'm curious, what's the difference between %s?\n", node->data);

                My_flush ();

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
                
                node->left->data = strdup (object);
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


                Print_colour (YELLOW, "Thank you, now I know more, next time I'll guess right.\n");
                break;
            }

            default:
                printf ("Incorrect entered command\n\n");
                return AKINATOR_GAME_ERR;

        }

        return 0;
    }

    else
    {
        printf ("%s?\n", node->data);
        printf ("Press Y - YES\nPress N - NO\n");

        My_flush ();
        
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