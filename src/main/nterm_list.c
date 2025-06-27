
#include "nterm_list.h"
#include "alloc.h"
#include "pointer_list.h"
#include "master_list.h"

nterm_list_t* create_nterm_list(void) {

    return (nterm_list_t*)create_ptr_list();
}

void destroy_nterm_list(void) {

    nterm_list_t* lst = get_master_list()->nterm_list;

    if(lst != NULL) {
        nterm_item_t* item;
        int mark = 0;
        while(NULL != (item = iterate_nterm_list(&mark)))
            destroy_nterm_item(item);

        destroy_ptr_list((pointer_list_t*)lst);
    }
}

void append_nterm_list(nterm_item_t* item) {

    append_ptr_list(get_master_list()->nterm_list, (void*)item);
}

nterm_item_t* iterate_nterm_list(int* mark) {

    return (nterm_item_t*)iterate_ptr_list(get_master_list()->nterm_list, mark);
}

nterm_item_t* index_nterm_list(int idx) {

    return (nterm_item_t*)index_ptr_list(get_master_list()->nterm_list, idx);
}

nterm_item_t* create_nterm_item(string_t* nterm, string_t* type, ast_node_t* node) {

    nterm_item_t* ptr = _ALLOC_TYPE(nterm_item_t);
    ptr->nterm = nterm;
    ptr->type = type;
    ptr->node = node;

    return ptr;
}

void destroy_nterm_item(nterm_item_t* item) {

    if(item != NULL) {
        destroy_string(item->nterm);
        destroy_string(item->type);
        _FREE(item);
    }
}

// the nterm and the type are the same lexical order
static int comp_nterm(void* p1, void* p2) {

    return comp_string(((nterm_item_t*)p1)->nterm, ((nterm_item_t*)p2)->nterm);
}

void sort_nterm_list(void) {

    sort_ptr_list(get_master_list()->nterm_list, comp_nterm);
}

// using the nterm or the type creates the same result
nterm_item_t* find_nterm(string_t* str) {

    nterm_item_t item;
    item.nterm = str;

    return find_ptr_list(get_master_list()->nterm_list, &item, comp_nterm);
}
