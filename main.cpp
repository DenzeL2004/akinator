#include <stdio.h>
#include <string>
#include <windows.h>

#include "akinator_game/akinator_game.h"
#include "src/log_info/log_errors.h"
#include "src/Generals_func/generals.h"

int main (int argc, char *argv[])
{
    system(("chcp " + std::to_string(CP_UTF8)).c_str());
 
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif 

    Tree tree = {};

    if (Tree_ctor (&tree))
    {
        Log_report ("ERROR: Ctor tree in main\n");
        Err_report ();
        return -1;
    }

    switch (argc)
    {
        case 1:
            break;

        case 2:
        {
            Read_tree_from_file (&tree, argv[1]);
            break;
        }

        default:
            Log_report ("Too many command line arguments\n");
            Err_report ();
            return -1;
    }
    
    Game (&tree);
    
    Save_tree_to_file (&tree);

    if (Tree_dtor (&tree))
    {
        Log_report ("ERROR: Dtor tree in main\n");
        Err_report ();
        return -1;
    }
    
    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif
}