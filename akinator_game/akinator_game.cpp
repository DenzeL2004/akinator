#include "../architecture/os_config.h"

#ifdef WINDOWS_USER

    #define  TX_USE_SPEAK
    #include "TXLib.h"

    #include <windows.h>

#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "akinator_game.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

#include "../src/stack/stack.h"
#include "../src/process_text/process_text.h"


void Phrase_akinator (const char *colour, const int voice_mode, const char *format, ...); 

static int Parting_akinator ();

static int Print_modes ();

static int Greeting_akinator ();



static int Play_akinator (Tree *tree);

static int Akinator (Tree *tree, Node *node);



static int Read_new_object (Node *node);

static int Read_string (char *str);


static int Print_database (Tree *tree);


static int Get_definition (Tree *tree);

static int Compare_objects (Tree *tree);

static int Print_similar (Stack *def_obj1, Stack *def_obj2);


static int Find_object (Node *node, const char *name_obj, Stack *def);

static int Print_definition (Stack *def, const char *colour = RESET); 

static int Make_string_definition (Stack *def, char* str_definition);


static int Save_nodes_recursive_to_file (const Node *node, FILE *fpout);


static int Read_nodes_recursive_from_buffer (Node *node, Text_info *text);

static int Read_node_from_buffer (Node *node, Text_info *text);

//======================================================================================

int Akinator_struct_ctor (Akinator_struct *akinator)
{
    assert (akinator != nullptr && "akinator is nullptr");

    if (Tree_ctor (&akinator->tree))
    {
        PROCESS_ERROR ("ERROR: Ctor tree in main\n");
        return AKINATOR_STRUCT_CTOR_ERR;
    }
    
    akinator->database = nullptr;

    return 0;
}

//======================================================================================

int Akinator_struct_dtor (Akinator_struct *akinator)
{
    assert (akinator != nullptr && "akinator is nullptr");

    if (Tree_dtor (&akinator->tree))
    {
        PROCESS_ERROR ("ERROR: Ctor tree in main\n");
        return AKINATOR_STRUCT_DTOR_ERR;
    }
    
    free (akinator->database);
    akinator->database = nullptr;

    return 0;
}

//======================================================================================

#define DEF_MODE(name_mode, ...)                \
        if (!strcmpi (#name_mode, cur_cmd))     \
        {                                       \
            __VA_ARGS__                         \
        }                                       \
        else                                    \
                                                        

int Game (Akinator_struct *akinator)
{
    assert (akinator != nullptr && "akinator is nullptr");

    Greeting_akinator ();

    Phrase_akinator (RESET, VOICE, "I have several modes of operation, what shall we do?\n");
    Print_modes ();

    while (true)
    {
        Phrase_akinator (RESET, VOICE, "Enter command: ");

        char cur_cmd[Max_command_buffer] = {0};

        if (scanf ("%s", cur_cmd) != 1)
        {
            PROCESS_ERROR ("Error reading the operating mode of the akinator\n");
            return PLAY_GAME_ERR;
        }

        if (!strcmpi ("GAME", cur_cmd))     
        {                                       
            if (Play_akinator (&akinator->tree))
            {
                PROCESS_ERROR ("Akinator error\n");
                return PLAY_GAME_ERR;
            }              
        }   

        else if (!strcmpi ("PRINT", cur_cmd))
        {
            if (Print_database (&akinator->tree))
            {
                PROCESS_ERROR ("Print database error\n");
                return DRAW_DATABASE_ERR;
            }
        }

        else if (!strcmpi ("DEFINITION", cur_cmd))     
        {                                       
            if (Get_definition (&akinator->tree))
            {
                PROCESS_ERROR ("Get definition error\n");
                return GET_DEFINITION_ERR;
            }              
        } 

        else if (!strcmpi ("COMPARISON", cur_cmd))     
        {                                       
            if (Compare_objects (&akinator->tree))
            {
                PROCESS_ERROR ("Get definition error\n");
                return COMPARE_OBJECT_ERR;
            }             
        } 

        else if (!strcmpi ("EXIT", cur_cmd))     
        {                                       
            break;      
        } 

        else if (!strcmpi ("MODES", cur_cmd))     
        {                                       
            Print_modes ();            
        } 
        
        else
            Phrase_akinator (RED, VOICE, "I don't known this commands!\n");
    }

    Parting_akinator ();

    return 0;
}

#undef DEF_MODE

//======================================================================================

void Phrase_akinator (const char *colour, int voice_mode, const char *format, ...) 
{ 
    assert (colour != nullptr && "colour is nullptr");
    assert (format != nullptr && "format is nullptr");

    va_list args = {0};
   
    va_start(args, format);
    
    char print_command [Max_command_buffer] = {0};

    vsprintf (print_command, format, args);
    Print_colour (colour, print_command);

    #ifdef USE_VOICE_MESSAGE

        if (voice_mode)
        {    
            char voice_command [Max_command_buffer] = {0};
            sprintf (voice_command, "%s%s%s", "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis'"
                                                                    " xml:lang='EN'>", print_command, "</speak>");

            #ifdef WINDOWS_USER
                txSpeak (voice_command);
            #endif
        }
    
    #endif
    va_end(args);

    return;                                                       
}

//======================================================================================

static int Greeting_akinator ()
{
    Phrase_akinator (YELLOW, VOICE, "Loading...\n");

    for (int time = 1; time <= 3; time++)
        Phrase_akinator (YELLOW, VOICE, "Connecting to the networ: %d%\n", time * 33);
    

    Phrase_akinator (GREEN, VOICE, "\nComplete 100%\n");

    Phrase_akinator (RESET, VOICE, "\nHello, my name is Akinator Tom.\n");
    Phrase_akinator (YELLOW, VOICE, "I'm glad you started me up, let's play!!!\n");
    
    return 0;
}

//======================================================================================

static int Print_modes ()
{
    printf ("\n");

    Phrase_akinator (RESET, SILENCE, "1. Game. Akinator will try to guess your hidden word.\n");
    Phrase_akinator (RESET, SILENCE, "2. Print. A picture of a tree with all current nodes will be drawn.\n");
    Phrase_akinator (RESET, SILENCE, "3. Definition. Tom, using his database, will define the given word.\n");
    Phrase_akinator (RESET, SILENCE, "4. Comparison. Tom, using his database, tries to compare objects.\n");
    Phrase_akinator (RESET, SILENCE, "5. Modes. Print mode List.\n");
    Phrase_akinator (RESET, SILENCE, "6. Exit. Finish the game.\n");
    

    printf ("\n");

    return 0;
} 

//======================================================================================

static int Parting_akinator ()
{
    Phrase_akinator (YELLOW, VOICE, "Have you already played it???\n");
    Phrase_akinator (BLUE,   VOICE, "I will miss you, come back soon\n");

    return 0;
}

//======================================================================================

static int Print_database (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    if (Draw_tree_graph (tree, "graph_img/cur_graph.png", DRAW_DATA))
    {
        PROCESS_ERROR ("Error in graph drawing\n");
        return DRAW_DATABASE_ERR;
    }

    if (system ("@temp\\graph_img\\cur_graph.png"))
    {
        PROCESS_ERROR ("Failed to open image\n");
        return DRAW_DATABASE_ERR;
    }

    return 0;
}

//======================================================================================

static int Get_definition (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    Phrase_akinator (RESET, VOICE, "Enter the name of the object you want to find\n");

    My_flush ();

    char name_obj[Max_command_buffer] = {0};
    if (Read_string (name_obj))
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    Stack def = {};

    if (Stack_ctor (&def, Min_stack_capacity))
    {
        PROCESS_ERROR ("Ctor stack error\n");
        return GET_DEFINITION_ERR;
    }


    if (Find_object (tree->root, name_obj, &def))
    {
        Phrase_akinator (YELLOW, VOICE, "Oh, I know what it is, listen.\n");
        Phrase_akinator (GREEN,  VOICE, "%s ", name_obj);
       
        if (Print_definition (&def, GREEN))
        {
            PROCESS_ERROR ("Error in definition entry\n");
            return GET_DEFINITION_ERR;
        }

        printf ("\n");
    }

    else
    {
        Phrase_akinator (BLUE, VOICE, "You will be surprised, "
                            "but I don't know what you thought of me, "
                            "it turned out awkwardly.\n");
    }


    if (Stack_dtor (&def))
    {
        PROCESS_ERROR ("Dtor stack error\n");        
        return GET_DEFINITION_ERR;
    }

    return 0;
}

//======================================================================================

static int Compare_objects (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");
    
    Phrase_akinator (RESET, VOICE, "Enter the names of the objects you want to compare\n");

    char name_obj[3][Max_command_buffer] = {0};
    Stack def_obj[3] = {};

    My_flush ();

    for (int ind = 1; ind <= 2; ind++)
    {
        if (Read_string (name_obj[ind]))
        {
            Log_report ("Error reading the name of object\n");
            Err_report ();
            return COMPARE_OBJECT_ERR;
        }

        if (Stack_ctor (&def_obj[ind], Min_stack_capacity))
        {
            PROCESS_ERROR ("Ctor stack error\n");
            return COMPARE_OBJECT_ERR;
        }

        if (!Find_object (tree->root, name_obj[ind], &def_obj[ind]))
        {
            Phrase_akinator (BLUE, VOICE, "\nI don't know of such %s.\n", name_obj[ind]);
            return 0;
        }
    }

    if (!strcmpi (name_obj[1], name_obj[2]))
    {
        Phrase_akinator (RESET, VOICE, "The same word was given\n");
        Phrase_akinator (GREEN, VOICE, "%s ", name_obj[1]);

        if (Print_definition (&def_obj[1], GREEN))
        {
            PROCESS_ERROR ("Error in definition entry\n");
            return GET_DEFINITION_ERR;
        }

        printf ("\n");

        return 0;
    }

    Phrase_akinator (RED,    VOICE, "%s ", name_obj[1]);
    Phrase_akinator (RESET,  VOICE, "and ");
    Phrase_akinator (YELLOW, VOICE, "%s ", name_obj[2]);
    Phrase_akinator (GREEN, VOICE, "they are similar in that they both ");

    if (Print_similar (&def_obj[1], &def_obj[2]))
    {
        PROCESS_ERROR ("Unable to print similar features of objects\n");
        return COMPARE_OBJECT_ERR;
    }

    int size_def[3] = {0};

    size_def[1] = Stack_get_size (&def_obj[1]);
    size_def[2] = Stack_get_size (&def_obj[2]);


    if (size_def[1] > 0 || size_def[2] > 0)
        Phrase_akinator (RESET, VOICE, "but ");


    for (int ind = 1; ind <= 2; ind++)
    {
        if (size_def[ind] > 0){
            char *colour = (char*) RESET;

            if (ind == 1) colour = (char*) RED;
            if (ind == 2) colour = (char*) YELLOW;

            Phrase_akinator (colour, VOICE, "%s ", name_obj[ind]);

            if (Print_definition (&def_obj[ind], colour))
            {
                PROCESS_ERROR ("Error in definition entry\n");
                return GET_DEFINITION_ERR;
            }
        }
    }
    

    printf ("\n");

    for (int ind = 1; ind <= 2; ind++)
    {
        if (Stack_dtor (&def_obj[ind]))
        {
            PROCESS_ERROR ("Dtor stack error\n");
            return GET_DEFINITION_ERR;
        }
    }

    return 0;
}

//======================================================================================

static int Print_similar (Stack *def_obj1, Stack *def_obj2)
{
    assert (def_obj1 != nullptr && "def_obj1 is nullptr");
    assert (def_obj2 != nullptr && "def_obj2 is nullptr");

    char str_definition[Max_definition_buffer] = {0};

    int size_def1 = Stack_get_size (def_obj1);
    int size_def2 = Stack_get_size (def_obj2);
    
    while (size_def1 > 0 && size_def1 > 0)
    {
        char *node_data1 = nullptr;
        char *node_data2 = nullptr;

        if (Stack_pop (def_obj1, &node_data1))
        {
            PROCESS_ERROR ("Removing an element from the stack failed\n");
            return PRINT_DEFINITION_ERR;
        }

        if (Stack_pop (def_obj2, &node_data2))
        {
            PROCESS_ERROR ("Removing an element from the stack failed\n");
            return PRINT_DEFINITION_ERR;
        }

        if (strcmpi (node_data1, node_data2))
        {
            Stack_push (def_obj1, node_data1);
            Stack_push (def_obj2, node_data2); 
            break;
        }

        sprintf (str_definition, "%s ", node_data1);
    
        size_def1--;
        size_def2--;
    }

    Phrase_akinator (GREEN, VOICE, "%s ", str_definition);

    return 0;
}

//======================================================================================

static int Print_definition (Stack *def, const char *colour)
{
    assert (def != nullptr && "Stack def is nullptr");

    char str_definition[Max_definition_buffer] = {0};

    if (Make_string_definition (def, str_definition))
    {
        PROCESS_ERROR ("Print definition error\n");
        return PRINT_DEFINITION_ERR;
    }

    Phrase_akinator (colour, VOICE, "%s ", str_definition);

    return 0;
}

//======================================================================================

static int Make_string_definition (Stack *def, char* str_definition)
{
    assert (def != nullptr && "Stack def is nullptr");

    int size = Stack_get_size (def);

    while (size > 0)
    {
        char *node_data = nullptr;
        if (Stack_pop (def, &node_data))
        {
            PROCESS_ERROR ("Removing an element from the stack failed\n");
            return MAKE_STR_DEFINITION;
        }
        
        sprintf (str_definition, "%s ", node_data);

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

    Phrase_akinator (RESET, VOICE, "Let's start the game\n");

    bool let_play = true;

    while (let_play)
    {
        printf ("\n");
        
        if (Akinator (tree, tree->root))
        {
            PROCESS_ERROR ("ERROR: Akinator function worked incorrectly\n");
            Tree_dump (tree, "PLAY AKINATOR");
            return PLAY_AKINATOR_ERR;
        }

        printf ("\n");
        Phrase_akinator (RESET, VOICE,   "Want to play again?\n");
        Phrase_akinator (RESET, SILENCE, "Entry Y - YES\nEntry N - NO\n");
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
                Phrase_akinator (RED, VOICE, "Incorrect command\n\n");
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
        Phrase_akinator (BLUE, VOICE, "I don't know who you asked me. Please, tell me, who is it?\n");

        if (Read_new_object (node))
        {
            PROCESS_ERROR ("Failed to read the new definition");
            return AKINATOR_GAME_ERR;
        }

        Phrase_akinator (YELLOW, VOICE, "Thank you, now I know more, next time I'll guess right.\n");

        return 0;
    }

    
    if (Is_leaf_node (node))
    {
        Phrase_akinator (RESET, VOICE, "It is %s?\n", node->data);
        Phrase_akinator (RESET, SILENCE, "Entry Y - YES\nEntry N - NO\n");

        My_flush ();

        switch (getchar ())
        {
            case 'Y':
                Phrase_akinator (YELLOW, VOICE, "Haha, I won!!!!\n");
                break;

            case 'N':
            {
                Phrase_akinator (BLUE, VOICE, "It seems I don't know who you thought of, who is it?\n");

                My_flush ();

                char object[Max_object_buffer] = {0};
                if (Read_string (object))
                {
                    PROCESS_ERROR ("Error reading from console\n");
                    return AKINATOR_GAME_ERR;
                }

                Phrase_akinator (RESET, VOICE, "I'm curious, what's the difference between %s?\n", node->data);

                char definition[Max_definition_buffer] = {0};
                if (Read_string (definition))
                {
                    PROCESS_ERROR ("Error reading from console\n");
                    return AKINATOR_GAME_ERR;
                }


                if (Add_node_sons (node))
                {
                    PROCESS_ERROR ("Error adding children of a node = |%p|", (char*) node);
                    return READ_NODE_ERR;
                }
                

                node->right->data = node->data;
                
                node->left->data = strdup (object);
                if (Check_nullptr (node->left->data))
                {
                    PROCESS_ERROR ("Memory allocation error\n");
                    return AKINATOR_GAME_ERR;
                }

                node->data = strdup (definition);
                if (Check_nullptr (node->data))
                {
                    PROCESS_ERROR ("Memory allocation error\n");
                    return AKINATOR_GAME_ERR;
                }

                Phrase_akinator (YELLOW, VOICE, "Thank you, now I know more, next time I'll guess right.\n");
                break;
            }

            default:
                Phrase_akinator (RED, VOICE, "Incorrect entered command\n\n");
                return AKINATOR_GAME_ERR;

        }

        return 0;
    }

    else
    {
        Phrase_akinator (RESET, VOICE, "%s?\n", node->data);
        Phrase_akinator (RESET, SILENCE, "Entry Y - YES\nEntry N - NO\n");

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

static int Read_new_object (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    My_flush ();

    char object[Max_object_buffer] = {0};
    if (Read_string (object))
    {
        PROCESS_ERROR ("Error reading from console\n");
        return AKINATOR_GAME_ERR;
    }
    
    node->data = strdup (object);
    if (Check_nullptr (node->data))
    {
        PROCESS_ERROR ("Memory allocation error\n");
        return AKINATOR_GAME_ERR;
    }

    return 0;
}

//======================================================================================

static int Read_string (char *str)
{
    assert (str != nullptr && "str is nullptr");

    if (!fgets (str, Max_command_buffer - 1, stdin))
    {
        PROCESS_ERROR ("Error reading from console\n");
        return READING_LINE_ERR;
    }

    char *pos = strchr (str, '\n');

    if (pos) *pos = '\0';
    
    return 0;
}

//======================================================================================

int Save_database_to_file (const Akinator_struct *akinator, const char *name_output_file)
{
    assert (akinator != nullptr && "akinator is nullptr");


    FILE *fpout = Open_file_ptr (name_output_file, "w");

    if (Check_nullptr (fpout)) 
    {
        PROCESS_ERROR ("Error opening output file named \"%s\"\n", name_output_file);
        return ERR_FILE_OPEN;
    }

    if (Save_nodes_recursive_to_file (akinator->tree.root, fpout))
    {
        PROCESS_ERROR ("Error saving a tree with a pointer to the root |%p|\n", (char*) akinator->tree.root);
        return SAVING_DATABASE_ERR;
    }

    if (Close_file_ptr (fpout)) 
    {
        PROCESS_ERROR ("Error close output file named \"%s\"\n", name_output_file);
        return ERR_FILE_CLOSE;
    }

    return 0;
}

//======================================================================================

static int Save_nodes_recursive_to_file (const Node *node, FILE *fpout)
{
    assert (node != nullptr && "node is nullptr");
    assert (fpout != nullptr && "fpout is nullptr");

    if (Is_leaf_node (node))
        fprintf (fpout, " { %s } ", node->data);

    else
    {
        fprintf(fpout, " { %s", node->data);

        if (!Check_nullptr (node->left)) 
        {
            if (Save_nodes_recursive_to_file(node->left, fpout))
                return SAVING_NODE_ERR;
        }

        if (!Check_nullptr (node->right)) 
        {
            if (Save_nodes_recursive_to_file(node->right, fpout))
                return SAVING_NODE_ERR;
        }

        fprintf(fpout, " } ");
    }

    return 0;
}

//======================================================================================

int Load_database (Akinator_struct *akinator, const char *name_input_file)
{
    assert (akinator != nullptr && "akinator is nullptr");
    assert (name_input_file != nullptr && "name_input_file is nullptr");

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
    {
        PROCESS_ERROR ("Error opening iputfile file named \"%s\"\n", name_input_file);
        return ERR_FILE_OPEN;
    }

    Text_info text = {};
    
    if (Text_read (fdin, &text))
    {
        PROCESS_ERROR ("Error reading into Text_info structure\n");
        return ERR_FILE_READING;
    }
    
    akinator->database = text.text_buf;
    text.pos = 0;

    if (Read_nodes_recursive_from_buffer (akinator->tree.root, &text))
    {   
        PROCESS_ERROR ("Error read a tree with a pointer to the root |%p|\n", 
                                                (char*) akinator->tree.root);
        return READ_DATABASE_ERR;
    }

    if (Close_file_discriptor (fdin))
    {
        PROCESS_ERROR ("Error close input file named \"%s\"\n", name_input_file);
        return ERR_FILE_CLOSE;
    }

    return 0;
}

//======================================================================================

static int Read_nodes_recursive_from_buffer (Node *node, Text_info *text)
{
    assert (text != nullptr && "text is nullptr");

    if (Read_node_from_buffer (node, text))
    {
        PROCESS_ERROR ("Error reading the value of node = |%p|", (char*) node);
        return READ_NODE_ERR;
    }
    
    if (*(text->text_buf + text->pos) == '}') 
    {
        text->pos++;
        return 0;
    }

    if (*(text->text_buf + text->pos) == '{') {
        
        if (Add_node_sons (node))
        {
            PROCESS_ERROR ("Error adding children of a node = |%p|", (char*) node);
            return READ_NODE_ERR;
        }

        if (Read_nodes_recursive_from_buffer (node->left, text))
        {
            PROCESS_ERROR ("Error reading the value of the left son = |%p|",
                                                        (char*) node->left);
            return READ_NODE_ERR;
        }

        if (Read_nodes_recursive_from_buffer (node->right, text))
        {
            PROCESS_ERROR ("Error reading the value of the right son = |%p|",
                                                        (char*) node->right);
            return READ_NODE_ERR;
        }

        int shift = 0;
        char symbol = 0;

        int result_scanned = sscanf (text->text_buf + text->pos, " %c %n", &symbol, &shift);
        if (result_scanned != 1)
        {
            PROCESS_ERROR ("Read error, was read: %d\n", result_scanned);
            return READ_NODE_ERR;
        }
        
//      "Error READ_NODE_ERR (-13): Read error, was read: 8"
      
        if (symbol == '}') 
        {
            text->pos += shift;
            return 0;
        }

        else 
        {
            PROCESS_ERROR ("Read error, undefined symbol, was read: |%c|\n", symbol);
            return READ_NODE_ERR;
        }

    }
    
    return READ_NODE_ERR;
}

//======================================================================================

static int Read_node_from_buffer (Node *node, Text_info *text)
{
    assert (text != nullptr && "text is nullptr");

    int shift = 0;
    char symbol = 0;
    
    int result_scanned = sscanf (text->text_buf + text->pos, " %c %n", &symbol, &shift);
    if (result_scanned != 1) 
    {
        PROCESS_ERROR ("Read error, was read: %d\n", result_scanned);
        return READ_NODE_ERR;
    }
    
    if (symbol != '{') 
    {
        PROCESS_ERROR ("Read error, undefined symbol, was read: |%c|\n", symbol);
        return READ_DATABASE_ERR;
    }

    text->pos += shift;
    char *node_data = (char*) (text->text_buf + text->pos);

    symbol = 0;
    int len_word = 0;

    while (symbol != '{' && symbol != '}') 
    {
        result_scanned = sscanf (text->text_buf + text->pos, "%*s%n %n%c", 
                                                    &len_word, &shift, &symbol);
        if (result_scanned != 1) 
        {
            PROCESS_ERROR ("Read error, was read: %d\n", result_scanned);
            return READ_NODE_ERR;
        }
        
        text->pos += shift;
    }
    
    *(text->text_buf + text->pos - shift + len_word) = '\0';
    
    node->data = node_data;

    return 0;
}

//======================================================================================