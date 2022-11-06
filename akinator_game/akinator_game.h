#ifndef _AKINATOR_GAME_H_
#define _AKINATOR_GAME_H_

#include "../src/tree/tree.h"
#include "../src/tree/draw_tree.h"

#define USE_VOICE_MESSAGE

static const int Delay_time = 300;

enum Akinator_func_err
{
    AKINATOR_GAME_ERR       = -1,
    PLAY_AKINATOR_ERR       = -2,

    PLAY_GAME_ERR           = -3,

    DRAW_DATABASE_ERR       = -4,
    GET_DEFINITION_ERR      = -5,
    COMPARE_OBJECT_ERR      = -6,

    PRINT_DEFINITION_ERR    = -7,
    READING_LINE_ERR        = -8,
};

int Game (Tree *tree);


#endif //#endif _AKINATOR_GAME_H_