/**
 * @file state_table.h
 */
#ifndef _STATE_TABLE_H_
#define _STATE_TABLE_H_

#include "array.h"

typedef array_t state_table_t;

typedef struct {
    unsigned int state_number;  // state number matches the index into the array
    unsigned int terminal;      // index of the terminal in the term_list_t
    unsigned int match_state;   // state to jump upon match
    unsigned int no_match_state;// state to jump upon not a match
} state_table_entry_t;


#endif /* _STATE_TABLE_H_ */
