
#include "master_list.h"
#include "errors.h"
#include "terms.h"
#include "alloc.h"

term_list_t* create_term_list(void) {

    return create_ptr_list();
}

void destroy_term_list(void) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    int mark = 0;
    term_elem_t* item;

    while(NULL != (item = iterate_term_list(&mark)))
        destroy_term_elem(item);

    destroy_ptr_list(get_master_list()->terms);
}

void add_term_list(string_t* name, string_t* type_str, int type) {

    append_ptr_list(get_master_list()->terms, create_term_elem(name, type_str, type));
}

term_elem_t* create_term_elem(string_t* name, string_t* type_str, int type) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    term_elem_t* term = _ALLOC_TYPE(term_elem_t);
    term->name = name;
    term->type_str = type_str;
    term->type = type;

    return term;
}

void destroy_term_elem(term_elem_t* ptr) {

    ASSERT(ptr != NULL, "attempt to destroy a NULL terminal symbol");

    destroy_string(ptr->name);
    destroy_string(ptr->type_str);
    _FREE(ptr);
}

void add_term_elem(term_elem_t* ptr) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    append_ptr_list(get_master_list()->terms, ptr);
}

term_elem_t* iterate_term_list(int* mark) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    return iterate_ptr_list(get_master_list()->terms, mark);
}

static int comp_func(void* ptr1, void* ptr2) {

    return comp_string(((term_elem_t*)ptr1)->type_str, ((term_elem_t*)ptr2)->type_str);
}

void sort_term_list(void) {

    sort_ptr_list(get_master_list()->terms, comp_func);
}

term_list_t* find_term(string_t* str) {

    term_elem_t elem;
    elem.type_str = str;

    return find_ptr_list(get_master_list()->terms, &elem, comp_func);
}
