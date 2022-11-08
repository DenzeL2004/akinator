#ifndef _AKINATOR_GAME_H_
#define _AKINATOR_GAME_H_

#include "../src/tree/tree.h"
#include "../src/tree/draw_tree.h"



//#define USE_VOICE_MESSAGE


enum Akinator_func_err
{
    AKINATOR_GAME_ERR           = -1,
    PLAY_AKINATOR_ERR           = -2,

    PLAY_GAME_ERR               = -3,

    DRAW_DATABASE_ERR           = -4,
    GET_DEFINITION_ERR          = -5,
    COMPARE_OBJECT_ERR          = -6,

    PRINT_DEFINITION_ERR        = -7,
    READING_LINE_ERR            = -8,
    MAKE_STR_DEFINITION         = -15,

    AKINATOR_STRUCT_CTOR_ERR    = -9,
    AKINATOR_STRUCT_DTOR_ERR    = -10,

    SAVING_NODE_ERR             = -11,
    SAVING_DATABASE_ERR         = -12,

    READ_NODE_ERR               = -13,
    READ_DATABASE_ERR           = -14,

};


enum Voice_mode
{
    SILENCE = 0,
    VOICE   = 1,
};

struct Akinator_struct
{
    char *database = nullptr;

    Tree tree = {};
};

int Akinator_struct_ctor (Akinator_struct *akinator);

int Akinator_struct_dtor (Akinator_struct *akinator);



int Game (Akinator_struct *akinator);


int Save_database_to_file (const Akinator_struct *akinator, const char *name_output_file = Default_output_file_name);

int Load_database (Akinator_struct *akinator, const char *name_input_file);


#endif //#endif _AKINATOR_GAME_H_