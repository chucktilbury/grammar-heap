/**
 * @file term_list.h
 *
 * @brief Manage the terminal list
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */

#ifndef _TERM_LIST_H_
#define _TERM_LIST_H_

#include "string_buffer.h"
#include "pointer_list.h"

typedef pointer_list_t term_list_t;

typedef struct {
    string_t* term;
    string_t* token;
    int type;
} term_item_t;

term_list_t* create_term_list(void);
void destroy_term_list(term_list_t* lst);
void append_term_list(term_list_t* lst, term_item_t* item);
term_item_t* iterate_term_list(term_list_t* lst, int* mark);
term_item_t* index_term_list(term_list_t* lst, int idx);
void sort_term_list(term_list_t* lst);
term_item_t* find_term(term_list_t* lst, const char* name);

term_item_t* create_term_item(string_t* term, string_t* tok, int type);
void destroy_term_item(term_item_t* item);

#endif /* _TERM_LIST_H_ */
