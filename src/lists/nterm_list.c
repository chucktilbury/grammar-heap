/**
 * @file nterm_list.c
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-06
 * @copyright Copyright (c) 2025
 */

#include "nterm_list.h"
#include "alloc.h"
#include "pointer_list.h"
#include "string_list.h"
//#include "nterm_rules.h"

nterm_list_t* create_nterm_list(void) {

    return (nterm_list_t*)create_ptr_list();
}

void destroy_nterm_list(nterm_list_t* lst) {

    if(lst != NULL) {
        nterm_item_t* item;
        int mark = 0;
        while(NULL != (item = iterate_nterm_list(lst, &mark)))
            destroy_nterm_item(item);

        destroy_ptr_list((pointer_list_t*)lst);
    }
}

void append_nterm_list(nterm_list_t* lst, nterm_item_t* item) {

    append_ptr_list((pointer_list_t*)lst, (void*)item);
}

nterm_item_t* iterate_nterm_list(nterm_list_t* lst, int* mark) {

    return (nterm_item_t*)iterate_ptr_list((pointer_list_t*)lst, mark);
}

nterm_item_t* index_nterm_list(nterm_list_t* lst, int idx) {

    return (nterm_item_t*)index_ptr_list((pointer_list_t*)lst, idx);
}

nterm_item_t* create_nterm_item(string_t* nterm, string_t* type, ast_node_t* node) {

    nterm_item_t* ptr = _ALLOC_TYPE(nterm_item_t);
    ptr->nterm = nterm;
    ptr->type = type;
    ptr->node = node;
    ptr->ds_names = create_ptr_list();
    ptr->rule_comment = create_string_list();

    return ptr;
}

void destroy_nterm_item(nterm_item_t* item) {

    if(item != NULL) {
        destroy_string(item->nterm);
        destroy_string(item->type);
        destroy_string_list(item->rule_comment);
        _FREE(item);
    }
}

// the nterm and the type are the same lexical order
static int comp_nterm(void* p1, void* p2) {

    return comp_string(((nterm_item_t*)p1)->nterm, ((nterm_item_t*)p2)->nterm);
}

void sort_nterm_list(nterm_list_t* lst) {

    sort_ptr_list((pointer_list_t*)lst, comp_nterm);
}

// using the nterm or the type creates the same result
nterm_item_t* find_nterm(nterm_list_t* lst, const char* str) {

    string_t* ptr = create_string(str);
    nterm_item_t* item = create_nterm_item(ptr, ptr, NULL);

    nterm_item_t* retv = NULL;
    int val = find_ptr_list((pointer_list_t*)lst, item, comp_nterm);
    if(val >= 0)
        retv = lst->buffer[val];

    destroy_string(ptr);
    _FREE(item);
    return retv;
}

void create_ds_name(nterm_item_t* item, string_t* name, int type) {

    nterm_ds_type_t* ptr = _ALLOC_TYPE(nterm_ds_type_t);
    ptr->name = name;
    ptr->type = type;

    append_ptr_list(item->ds_names, (void*)ptr);
}
