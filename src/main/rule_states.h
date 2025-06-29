
#ifndef _RULE_STATES_H_
#define _RULE_STATES_H_

#include "tokens.h"

typedef enum {
    TOKEN_MATCH,
    BEGIN_LOOP,
    END_LOOP,
    CODE_MATCH,
} rule_state_type_t;

typedef struct _rule_state_t_ {
    int state;
    rule_state_type_t type;
    token_t* token;
    string_t* name;
    struct _rule_state_t_* next;
    struct _rule_state_t_* prev;
} rule_state_t;

typedef struct {
    rule_state_t* first;
    rule_state_t* last;
    rule_state_t* crnt;
} rule_state_list_t;

// uses definition in master_list
void build_rule_states(void);

void init_rule_state_list(void);
void destroy_rule_state_list(void);
void append_rule_state_list(rule_state_t* state);
void insert_rule_state_list(rule_state_t* state, rule_state_t* node);

rule_state_t* create_rule_state(rule_state_type_t type, string_t* name, token_t* tok);

#endif /* _RULE_STATES_H_ */
