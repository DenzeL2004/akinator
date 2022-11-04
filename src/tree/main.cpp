#include <stdio.h>

#include "tree.h"
#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"

int main ()
{
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
    
    do 
    {
        Akinator (&tree, tree.root);

        Tree_dump (&tree, "FROM MAIN");

        printf ("\n\n");
        printf ("Want to play again?\n");
        printf ("Press Y - YES\nPress N - NO\n");


        Mu_flush ();


    }while (getchar () == 'Y');
    
    

    

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