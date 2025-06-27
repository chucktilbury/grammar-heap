
#include "term_list.h"
#include "alloc.h"
#include "tokens.h"
#include "string_buffer.h"
#include "master_list.h"

term_list_t* create_term_list(void) {

    return (term_list_t*)create_ptr_list();
}

void destroy_term_list(void) {

    term_list_t* lst = get_master_list()->term_list;
    if(lst != NULL) {
        term_item_t* item;
        int mark = 0;
        while(NULL != (item = iterate_term_list(&mark)))
            destroy_term_item(item);

        destroy_ptr_list((pointer_list_t*)lst);
    }
}

void append_term_list(term_item_t* item) {

    term_item_t* temp;
    int mark = 0;
    while(NULL != (temp = iterate_term_list(&mark)))
        if(!comp_string(temp->tok->ptype, item->tok->ptype))
            return;

    append_ptr_list(get_master_list()->term_list, (void*)item);
}

term_item_t* iterate_term_list(int* mark) {

    return (term_item_t*)iterate_ptr_list(get_master_list()->term_list, mark);
}

term_item_t* index_term_list(int idx) {

    return (term_item_t*)index_ptr_list(get_master_list()->term_list, idx);
}

term_item_t* create_term_item(token_t* tok, int type) {

    term_item_t* ptr = _ALLOC_TYPE(term_item_t);
    ptr->tok = tok;
    ptr->type = type;

    return ptr;
}

void destroy_term_item(term_item_t* item) {

    if(item != NULL) {
        destroy_token(item->tok);
        _FREE(item);
    }
}

static int comp_term(void* p1, void* p2) {

    return comp_string(((term_item_t*)p1)->tok->ptype, ((term_item_t*)p2)->tok->ptype);
}

void sort_term_list(void) {

    sort_ptr_list(get_master_list()->term_list, comp_term);
}
