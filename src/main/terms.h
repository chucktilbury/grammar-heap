/*
 * Handle terminal symbols
 */
#ifndef _TERMS_H_
#define _TERMS_H_

#include "string_buffer.h"
#include "pointer_list.h"

typedef pointer_list_t term_list_t;

typedef struct {
    string_t* name;
    string_t* type_str;
    int type;
} term_elem_t;

// terminal list
term_list_t* create_term_list(void);
void destroy_term_list(void);
void add_term_list(string_t* name, string_t* type_str, int type);

// terminal elements
term_elem_t* create_term_elem(string_t* name, string_t* type_str, int type);
void destroy_term_elem(term_elem_t* ptr);
void add_term_elem(term_elem_t* ptr);
term_elem_t* iterate_term_list(int* mark);

#endif /* _TERMS_H_ */
