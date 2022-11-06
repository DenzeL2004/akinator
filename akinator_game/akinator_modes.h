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
    if (Get_definition (tree))
    {
        Log_report ("GEet definition error\n");
        Err_report ();
        return GET_DEFINITION_ERR;
    }
})

DEF_MODE (COMPARISON, {
    if (Compare_objects (tree))
    {
        Log_report ("GEet definition error\n");
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