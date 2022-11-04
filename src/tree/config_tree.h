#ifndef _TREE_CONFIG_H_
#define _TREE_CONFIG_H_

#include <stdint.h>

typedef char elem_t;


const int Poison_ptr  = -126;   //<- Written to a pointer when the list is cleared

#define USE_LOG                 //<- connect when we use logs


#define GRAPH_DUMP

#define ELEM_T_SPEC "s"            //<- specifier character to print elem

#endif  //endif _TREE_CONFIG_H_