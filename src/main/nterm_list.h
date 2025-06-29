
#ifndef _NTERM_LIST_H_
#define _NTERM_LIST_H_

#include "ast.h"

typedef pointer_list_t nterm_list_t;

typedef struct {
    string_t* nterm;
    string_t* type;
    ast_node_t* node;
    int ref_count;
    int is_recursive;
} nterm_item_t;

nterm_list_t* create_nterm_list(void);
void destroy_nterm_list(void);
void append_nterm_list(nterm_item_t* item);
nterm_item_t* iterate_nterm_list(int* mark);
nterm_item_t* index_nterm_list(int idx);
void sort_nterm_list(void);
nterm_item_t* find_nterm(string_t* name);

nterm_item_t* create_nterm_item(string_t* nterm, string_t* type, ast_node_t* node);
void destroy_nterm_item(nterm_item_t* item);

void create_ds_name(nterm_item_t* item, string_t* name, int type);

#endif /* _NTERM_LIST_H_ */
