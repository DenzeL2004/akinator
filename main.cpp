#include "architecture/os_config.h"

#include <stdio.h>
#include <string>

#ifdef WINDOWS_USER
    #include <windows.h>
#endif

#include "akinator_game/akinator_game.h"
#include "src/log_info/log_errors.h"
#include "src/Generals_func/generals.h"


int main (int argc, char *argv[])
{
    #ifdef WINDOWS_USER
        system(("chcp " + std::to_string(CP_UTF8)).c_str());
    #endif
    
 
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif 
    

    Akinator_struct akinator = {};

    if (Akinator_struct_ctor (&akinator))
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
            Load_database (&akinator, argv[1]);
            break;
        }

        default:
            Log_report ("Too many command line arguments\n");
            Err_report ();
            return -1;
    }

    Game (&akinator);
    
    Save_database_to_file (&akinator);

    if (Akinator_struct_ctor (&akinator))
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