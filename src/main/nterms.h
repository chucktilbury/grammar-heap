/*
 * Handle non-terminal symbols
 */
#ifndef _NTERMS_H_
#define _NTERMS_H_

#include "string_buffer.h"
#include "pointer_list.h"
#include "ast.h"

typedef pointer_list_t nterm_list_t;

typedef struct {
    string_t* name;
    ast_alternative_list_t* alts;
    int ref_count;
} nterm_elem_t;

// non-terminal list
nterm_list_t* create_nterm_list(void);
void destroy_nterm_list(void);
void add_nterm_list(string_t* name, ast_alternative_list_t* alts);

// non-terminal elements
nterm_elem_t* create_nterm_elem(string_t* name, ast_alternative_list_t* alts);
void destroy_nterm_elem(nterm_elem_t* ptr);
void add_nterm_elem(nterm_elem_t* ptr);
nterm_elem_t* iterate_nterm_list(int* mark);
void sort_nterm_list(void);
nterm_elem_t* find_nterm(string_t* str);

#endif /* _NTERMS_H_ */

