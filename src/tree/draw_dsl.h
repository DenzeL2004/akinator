#ifndef _DRAW_DSL_H_
#define _DRAW_DSL_H_

#define PRINT_SLASH(fp, byte)                               \
    if (node_mode & (~byte)) fprintf (fp, "|")

#define DELETE_BYTE_FROM_MODE(byte)                         \
    node_mode &= (~byte)

#endif  //#endif _DRAW_DSL_H_