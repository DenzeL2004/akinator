DEF_MODE (GAME, {
    if (Play_akinator (tree))
    {
        Log_report ("Akinator error\n");
        Err_report ();
        return PLAY_GAME_ERR;
    }
})

DEF_MODE (PRINT, {
    if (Print_database (tree))
    {
        Log_report ("Print database error\n");
        Err_report ();
        return DRAW_DATABASE_ERR;
    }
})

DEF_MODE (DEFINITION, {
    Phrase_akinator (RESET, "Enter the name of the object you want to find\n");

    My_flush ();

    char name_obj[Max_command_buffer] = {0};
    if (Read_string (name_obj))
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }

    if (Get_definition (tree, name_obj))
    {
        Log_report ("Get definition error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }
})

DEF_MODE (COMPARISON, {
    Phrase_akinator (RESET, "Enter the names of the objects you want to compare\n");

    My_flush ();

    char name_obj1[Max_command_buffer] = {0};
    if (Read_string (name_obj1))
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return COMPARE_OBJECT_ERR;
    }


    char name_obj2[Max_command_buffer] = {0};
    if (Read_string (name_obj2))
    {
        Log_report ("Error reading the name of object\n");
        Err_report ();
        return COMPARE_OBJECT_ERR;
    }

    if (Compare_objects (tree, name_obj1, name_obj2))
    {
        Log_report ("Get definition error\n");
        Err_report ();
        return COMPARE_OBJECT_ERR;
    }
})

DEF_MODE (EXIT, {
    break;
})

DEF_MODE (MODES, {
    Print_modes ();
})