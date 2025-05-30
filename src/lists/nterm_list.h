/**
 * @file nterm_list.h
 *
 * @brief Manage non-terminals data structure list.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */

#ifndef _NTERM_LIST_H_
#define _NTERM_LIST_H_

#include "ast.h"
#include "string_list.h"

typedef pointer_list_t nterm_list_t;

// data structure for ds_names
typedef struct {
    string_t* name;
    int type; // defined in parser.y as a token
} nterm_ds_type_t;

typedef struct {
    string_t* nterm;
    string_t* type;
    ast_node_t* node;
    pointer_list_t* ds_names;
    // ptr_list_t* rule_states;
    string_list_t* rule_comment;
} nterm_item_t;

nterm_list_t* create_nterm_list(void);
void destroy_nterm_list(nterm_list_t* lst);
void append_nterm_list(nterm_list_t* lst, nterm_item_t* item);
nterm_item_t* iterate_nterm_list(nterm_list_t* lst, int* mark);
nterm_item_t* index_nterm_list(nterm_list_t* lst, int idx);
void sort_nterm_list(nterm_list_t* lst);
nterm_item_t* find_nterm(nterm_list_t* lst, const char* name);

nterm_item_t* create_nterm_item(string_t* nterm, string_t* type, ast_node_t* node);
void destroy_nterm_item(nterm_item_t* item);

void create_ds_name(nterm_item_t* item, string_t* name, int type);

#endif /* _NTERM_LIST_H_ */
