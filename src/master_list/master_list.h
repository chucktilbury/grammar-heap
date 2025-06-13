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
    nterm_item_t* first_nterm;
    nterm_item_t* crnt_nterm;
    nterm_list_t* nterm_list;
    term_list_t* term_list;
    string_t* pre_text;
    string_t* pre_code;
    string_t* post_code;
} master_list_t;

extern master_list_t* master_list;

master_list_t* create_master_list(void);
void destroy_master_list(master_list_t* lst);
void dump_master_list(void);
void make_raw_lists(void);

#endif /* _LISTS_H_ */
