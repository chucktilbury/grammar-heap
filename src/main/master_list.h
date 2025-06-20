/*
 * Master List
 *
 * This is the precursor to the state machine.
 */
#ifndef _MASTER_LIST_H_
#define _MASTER_LIST_H_

#include "pointer_list.h"
#include "ast.h"

typedef struct {
    ast_grammar_t* root_node;
    pointer_list_t* nterms;
    pointer_list_t* terms;
    string_t* precode;
    string_t* postcode;
    string_t* pretext;
    string_t* headertext;
} master_list_t;

void init_master_list(void);
void create_master_list(void);
void destroy_master_list(void);
void set_master_list_root_node(void* node);
void* get_master_list_root_node(void);
master_list_t* get_master_list(void);
void dump_master_list(void);

#endif /* _MASTER_LIST_H_ */


