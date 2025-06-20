
#include "master_list.h"
#include "errors.h"
#include "nterms.h"
#include "alloc.h"

nterm_list_t* create_nterm_list(void) {

    return create_ptr_list();
}

void destroy_nterm_list(void) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    int mark = 0;
    nterm_elem_t* item;

    while(NULL != (item = iterate_nterm_list(&mark)))
        destroy_nterm_elem(item);

    destroy_ptr_list(get_master_list()->nterms);
}

void add_nterm_list(string_t* name, ast_alternative_list_t* alts) {

    append_ptr_list(get_master_list()->nterms, create_nterm_elem(name, alts));
}

nterm_elem_t* create_nterm_elem(string_t* name, ast_alternative_list_t* alts) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    nterm_elem_t* nterm = _ALLOC_TYPE(nterm_elem_t);
    nterm->alts = alts;
    nterm->name = name;

    return nterm;
}

void destroy_nterm_elem(nterm_elem_t* ptr) {

    ASSERT(ptr != NULL, "attempt to destroy NULL non-terminal symbol");

    // Note that the AST nodes are destroyed by the AST module
    destroy_string(ptr->name);
    _FREE(ptr);
}

void add_nterm_elem(nterm_elem_t* ptr) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    append_ptr_list(get_master_list()->nterms, ptr);
}

nterm_elem_t* iterate_nterm_list(int* mark) {

    ASSERT(get_master_list() != NULL, "master list is not initialized");

    return iterate_ptr_list(get_master_list()->nterms, mark);
}

