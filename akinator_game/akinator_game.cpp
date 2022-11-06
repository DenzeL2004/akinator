#define  TX_USE_SPEAK
#include "TXLib.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "windows.h"


#include "akinator_game.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"
#include "../src/stack/stack.h"



void Phrase_akinator (const char *colour, const char *format, ...); 

static int Parting_akinator ();

static int Print_modes ();

static int Greeting_akinator ();



static int Play_akinator (Tree *tree);

static int Akinator (Tree *tree, Node *node);


static int Read_string (char *str);

static int Print_database (Tree *tree);


static int Get_definition (Tree *tree, const char *name_obj);

static int Compare_objects (Tree *tree, const char *name_obj1, const char *name_obj2);



static int Find_object (Node *node, const char *name_obj, Stack *def);

static int Print_definition (Stack *def, const char *colour = RESET);         

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

    Phrase_akinator (RESET, "I have several modes of operation, what shall we do?\n");
    Print_modes ();

    while (true)
    {
        Phrase_akinator (RESET, "Enter command: ");

        char cur_cmd[Max_command_buffer] = {0};

        if (scanf ("%s", cur_cmd) != 1)
        {
            Log_report ("Error reading the operating mode of the akinator\n");
            Err_report ();
            return PLAY_GAME_ERR;
        }

        #include "akinator_modes.h"

        /*else*/
            Phrase_akinator (RED, "I don't known this commands!\n");
    }

    Parting_akinator ();

    return 0;
}

#undef DEF_MODE

//======================================================================================

void Phrase_akinator (const char *colour, const char *format, ...) 
{ 
    assert (colour != nullptr && "colour is nullptr");
    assert (format != nullptr && "format is nullptr");

    va_list args = {0};
   
    va_start(args, format);
    
    char print_command [Max_command_buffer] = {0};

    vsprintf (print_command, format, args);
    Print_colour (colour, print_command);

    #ifdef USE_VOICE_MESSAGE
    
        char voice_command [Max_command_buffer] = {0};
        sprintf (voice_command, "%s%s%s", "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis'"
                                                                " xml:lang='EN'>", print_command, "</speak>");

        txSpeak (voice_command);
    
    #endif
    va_end(args);

    return;                                                       
}

//======================================================================================

static int Greeting_akinator ()
{
    Phrase_akinator (YELLOW, "Loading...\n");

    for (int time = 1; time <= 3; time++)
        Phrase_akinator (YELLOW, "Connecting to the networ: %d%\n", time * 33);
    

    Phrase_akinator (GREEN, "\nComplete 100%\n");

    Phrase_akinator (RESET, "\nHello, my name is Akinator Tom.\n");
    Phrase_akinator (YELLOW, "I'm glad you started me up, let's play!!!\n");
    
    return 0;
}

//======================================================================================

static int Print_modes ()
{
    printf ("\n");

    Phrase_akinator (RESET, "1. Game. Akinator will try to guess your hidden word.\n");
    Phrase_akinator (RESET, "2. Print. A picture of a tree with all current nodes will be drawn.\n");
    Phrase_akinator (RESET, "3. Definition. Tom, using his database, will define the given word.\n");
    Phrase_akinator (RESET, "4. Comparison. Tom, using his database, tries to compare objects.\n");
    Phrase_akinator (RESET, "5. Exit. Finish the game.\n");
    Phrase_akinator (RESET, "6. Modes. Print mode List.\n");

    printf ("\n");

    return 0;
} 

//======================================================================================

static int Parting_akinator ()
{
    Phrase_akinator (YELLOW, "Have you already played it???\n");
    Phrase_akinator (BLUE, "I will miss you, come back soon\n");

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

static int Get_definition (Tree *tree, const char *name_obj)
{
    assert (tree != nullptr && "tree is nullptr");
    assert (name_obj != nullptr && "name_obj is nullptr");

    Stack def = {};

    if (Stack_ctor (&def, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }


    if (Find_object (tree->root, name_obj, &def))
    {
        Phrase_akinator (YELLOW, "Oh, I know what it is, listen.\n");
       
        Phrase_akinator (GREEN, "%s ", name_obj);
       
        if (Print_definition (&def, GREEN))
        {
            Log_report ("Error in definition entry\n");
            Err_report ();
            return GET_DEFINITION_ERR;
        }

        printf ("\n");
    }

    else
    {
        Phrase_akinator (BLUE, "You will be surprised, "
                            "but I don't know what you thought of me, "
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

static int Compare_objects (Tree *tree, 
                            const char *name_obj1, const char *name_obj2)
{
    assert (tree != nullptr && "tree is nullptr");
    assert (name_obj1 != nullptr && "name_obj1 is nullptr");
    assert (name_obj2 != nullptr && "name_obj2 is nullptr");

    if (!strcmpi (name_obj1, name_obj2))
    {
        Phrase_akinator (RESET, "\nYou asked the same. I can define an object.\n");
        
        if (Get_definition (tree, name_obj1))
        {
            Log_report ("GEet definition error\n");
            Err_report ();
            return GET_DEFINITION_ERR;
        }

        return 0;
    }

    Stack def_obj1 = {};

    if (Stack_ctor (&def_obj1, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return COMPARE_OBJECT_ERR;
    }

    if (!Find_object (tree->root, name_obj1, &def_obj1))
    {
        Phrase_akinator (BLUE, "\nI don't know of such %s.\n", name_obj1);
        return 0;
    }


    Stack def_obj2 = {};

    if (Stack_ctor (&def_obj2, Min_stack_capacity))
    {
        Log_report ("Ctor stack error\n");
        Err_report ();
        return COMPARE_OBJECT_ERR;
    }

    if (!Find_object (tree->root, name_obj2, &def_obj2))
    {
        Phrase_akinator (BLUE, "\nI don't know of such %s.\n", name_obj2);
        return 0;
    }


    int size_def1 = Stack_get_size (&def_obj1);
    int size_def2 = Stack_get_size (&def_obj2);

    Phrase_akinator (GREEN, "%s and %s they are similar in that they both ", 
                                                  name_obj1, name_obj2);

    while (size_def1 > 0 && size_def2 > 0)
    {
        char *node_data1 = nullptr;
        char *node_data2 = nullptr;

        Stack_pop (&def_obj1, &node_data1);
        Stack_pop (&def_obj2, &node_data2);

        if (strcmpi (node_data1, node_data2))
        {
            Stack_push (&def_obj1, node_data1);
            Stack_push (&def_obj2, node_data2); 
            break;
        }

        Phrase_akinator (GREEN, "%s ", node_data1);
    
        size_def1--;
        size_def2--;
    }

    if (size_def1 > 0 || size_def2 > 0)
        Phrase_akinator (RED, " but ");

    if (size_def1 > 0){
        Phrase_akinator (RED, "%s ", name_obj1);

        if (Print_definition (&def_obj1, RED))
        {
            Log_report ("Error in definition entry\n");
            Err_report ();
            return GET_DEFINITION_ERR;
        }
    }


    if (size_def2 > 0){
        Phrase_akinator (RED, "%s ", name_obj2);

        if (Print_definition (&def_obj2, RED))
        {
            Log_report ("Error in definition entry\n");
            Err_report ();
            return GET_DEFINITION_ERR;
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

static int Print_definition (Stack *def, const char *colour)
{
    assert (def != nullptr && "Stack def is nullptr");

    int size = Stack_get_size (def);

    while (size > 0)
    {
        char *node_data = nullptr;
        if (Stack_pop (def, &node_data))
        {
            Log_report ("Removing an element from the stack failed\n");
            Err_report ();
            return PRINT_DEFINITION_ERR;
        }

        Phrase_akinator (colour, "%s ", node_data);
        size--;
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
            return 1;
        }

    if (!Check_nullptr (node->right))
        if (Find_object (node->right, name_obj, def))
        {
            Stack_push (def, node->data);
            Stack_push (def, "not");
            return 1;
        }

    return 0;
}

static int Play_akinator (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    Phrase_akinator (RESET, "Let's start the game\n");

    bool let_play = true;

    while (let_play)
    {
        printf ("\n");
        
        if (Akinator (tree, tree->root))
        {
            Log_report ("ERROR: Akinator function worked incorrectly\n");
            Err_report ();
            Tree_dump (tree, "PLAY AKINATOR");

            return PLAY_AKINATOR_ERR;
        }

        printf ("\n");
        Phrase_akinator (RESET, "Want to play again?\n");
        Phrase_akinator (RESET, "Press Y - YES\nPress N - NO\n");
        printf ("\n");

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
                Phrase_akinator (RED, "Incorrect command\n\n");
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
        Phrase_akinator (BLUE, "I don't know who you asked me. Please, tell me, who is it?\n");

        My_flush ();

        char object[Max_object_buffer] = {0};
        if (Read_string (object))
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

        Phrase_akinator (YELLOW, "Thank you, now I know more, next time I'll guess right.\n");

        return 0;
    }

    
    if (Is_leaf_node (node))
    {
        Phrase_akinator (RESET, "It is %s?\n", node->data);
        Phrase_akinator (RESET, "Press Y - YES\nPress N - NO\n");

        My_flush ();

        switch (getchar ())
        {
            case 'Y':
                Phrase_akinator (YELLOW, "Haha, I won!!!!\n");
                break;

            case 'N':
            {
                Phrase_akinator (BLUE, "It seems I don't know who you thought of, who is it?\n");

                My_flush ();

                char object[Max_object_buffer] = {0};
                if (Read_string (object))
                {
                    Log_report ("Error reading from console\n");
                    Err_report ();
                    return AKINATOR_GAME_ERR;
                }

                Phrase_akinator (RESET, "I'm curious, what's the difference between %s?\n", node->data);

                char definition[Max_definition_buffer] = {0};
                if (Read_string (definition))
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

                Phrase_akinator (YELLOW, "Thank you, now I know more, next time I'll guess right.\n");
                break;
            }

            default:
                Phrase_akinator (RED, "Incorrect entered command\n\n");
                return AKINATOR_GAME_ERR;

        }

        return 0;
    }

    else
    {
        Phrase_akinator (RESET, "%s?\n", node->data);
        Phrase_akinator (RESET, "Press Y - YES\nPress N - NO\n");

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

static int Read_string (char *str)
{
    assert (str != nullptr && "str is nullptr");

    if (!fgets (str, Max_command_buffer - 1, stdin))
    {
        Log_report ("Error reading from console\n");
        Err_report ();
        return READING_LINE_ERR;
    }

    char *pos = strchr (str, '\n');

    if (pos) *pos = '\0';
    
    return 0;
}

//======================================================================================