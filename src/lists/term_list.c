/**
 * @file term_list.c
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */

#include "term_list.h"
#include "alloc.h"

term_list_t* create_term_list(void) {

    return (term_list_t*)create_ptr_list();
}

void destroy_term_list(term_list_t* lst) {

    if(lst != NULL) {
        term_item_t* item;
        int mark = 0;
        while(NULL != (item = iterate_term_list(lst, &mark)))
            destroy_term_item(item);

        destroy_ptr_list((ptr_list_t*)lst);
    }
}

void append_term_list(term_list_t* lst, term_item_t* item) {

    term_item_t* temp;
    int mark = 0;
    while(NULL != (temp = iterate_term_list(lst, &mark)))
        if(!comp_string(temp->token, item->token))
            return;

    append_ptr_list((ptr_list_t*)lst, (void*)item);
}

term_item_t* iterate_term_list(term_list_t* lst, int* mark) {

    return (term_item_t*)iterate_ptr_list((ptr_list_t*)lst, mark);
}

term_item_t* index_term_list(term_list_t* lst, int idx) {

    return (term_item_t*)index_ptr_list((ptr_list_t*)lst, idx);
}

term_item_t* create_term_item(string_t* term, string_t* tok, int type) {

    term_item_t* ptr = _ALLOC_TYPE(term_item_t);
    ptr->term = term;
    ptr->token = tok;
    ptr->type = type;

    return ptr;
}

void destroy_term_item(term_item_t* item) {

    if(item != NULL) {
        destroy_string(item->term);
        destroy_string(item->token);
        _FREE(item);
    }
}

static int comp_term(void* p1, void* p2) {

    return comp_string(((term_item_t*)p1)->term, ((term_item_t*)p2)->term);
}

void sort_term_list(term_list_t* lst) {

    sort_ptr_list((ptr_list_t*)lst, comp_term);
}

//
term_item_t* find_term(term_list_t* lst, const char* str) {

    // need to match the types for the search routine
    string_t* ptr = create_string(str);
    term_item_t* item = create_term_item(ptr, ptr, 0);

    term_item_t* retv = NULL;
    int val = find_ptr_list((ptr_list_t*)lst, item, comp_term);
    if(val >= 0)
        retv = lst->buffer[val];

    destroy_string(ptr);
    _FREE(item);
    return retv;
}
