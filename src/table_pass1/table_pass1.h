/**
 * @file table_pass1.h
 */
#ifndef _TABLE_PASS1_H_
#define _TABLE_PASS1_H_

#include "nterm_list.h"
#include "term_list.h"
#include "pointer_list.h"

// This is the representation of a non-terminal
typedef pointer_list_t state_ptr_table_t;

// This is a list of non-terminal representations
typedef pointer_list_t state_table_t;

typedef enum {
    // always nterm, always match only
    STATE_START,
    // always null value, always match only
    STATE_END,
    // must have match and no match, or error
    STATE_TERM,
    // may have match or no match. Reference count may be non-zero.
    STATE_NTERM,
    // reference count must be zero. must be match only.
    STATE_CODE,
    // state to go to when there is an error.
    STATE_ERROR,
} state_type_t;

typedef struct _state_ptr_t_ {
    state_type_t type;
    int reference_cnt; // This node is referenced by another.

    union {
        term_item_t* term;   // This terminal. Otherwise NULL.
        nterm_item_t* nterm; // This non-terminal. Otherwise NULL.
        string_t* code;      // This code node. Otherwise NULL.
    } value;

    struct _state_ptr_t_* match;    // Next state if the term is a match.
    struct _state_ptr_t_* no_match; // Next state if the term is not a match.
    struct _state_ptr_t_* error;    // Next state if an error match.
} state_ptr_t;

state_ptr_table_t* create_state_ptr_table(void);
void destroy_state_ptr_table(state_ptr_table_t* tab);

state_table_t* create_state_table(void);
void destroy_state_table(state_table_t* tab);

state_ptr_t* create_state_ptr(state_type_t type);
void destroy_state_ptr(state_ptr_t* ptr);

void create_table_pass1(void);


#endif /* _TABLE_PASS1_H_ */

