/**
 * @file raw_table.h
 */
#ifndef _RAW_TABLE_H_
#define _RAW_TABLE_H_

#include <stdint.h>

#include "nterm_list.h"
#include "term_list.h"
#include "pointer_list.h"

typedef pointer_list_t raw_state_table_t;

typedef struct _raw_state_item_t_ {
    term_item_t* terminal;
    uint32_t reference_cnt;
    struct _raw_state_item_t_* match;
    struct _raw_state_item_t_* no_match;
} raw_state_item_t;

typedef struct _raw_state_t_ {
    nterm_item_t* item;
    raw_state_item_t* rs_table;
} raw_state_t;

raw_state_table_t* init_raw_table(void);
void destroy_raw_table(raw_state_table_t* tab);

raw_state_item_t* create_state_item()

#endif /* _RAW_TABLE_H_ */
