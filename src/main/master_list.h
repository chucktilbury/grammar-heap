/**
 * @file lists.h
 *
 * @brief Traverse the AST to create the raw lists
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */
#ifndef _LISTS_H_
#define _LISTS_H_

#include "string_buffer.h"
#include "nterm_list.h"
#include "term_list.h"

typedef struct {
    ast_node_t* root_node;
    nterm_item_t* first_nterm;
    nterm_item_t* crnt_nterm;
    nterm_list_t* nterm_list;
    term_list_t* term_list;
    string_t* pre_text;
    string_t* post_text;
    string_t* pre_code;
    string_t* post_code;
    string_t* requires;
    string_t* provides;
    string_t* term_def;
    string_t* nterm_def;
} master_list_t;

void init_master_list(void);
void create_master_list(void);
void destroy_master_list(master_list_t* lst);
void dump_master_list(void);
void make_raw_lists(void);
master_list_t* get_master_list(void);
void* get_root_node(void);
void set_root_node(void* ptr);

#endif /* _LISTS_H_ */

