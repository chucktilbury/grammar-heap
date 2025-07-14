#ifndef _PARSER_H_
#define _PARSER_H_

#include "string_buffer.h"
#include "pointer_list.h"
#include "tokens.h"

typedef struct {
    token_t* name;
    pointer_list_t* expr; // list of token_t*
} rule_t;

typedef struct {
    string_t* pretext;
    string_t* posttext;
    string_t* precode;
    string_t* postcode;
    string_t* term_def;
    string_t* nterm_def;
    string_t* provides;
    string_t* requires;
    rule_t* start_rule;
    rule_t* crnt_rule;
    pointer_list_t* rule_list;
} parser_state_t;

void init_parser(void);
int parser(void);

rule_t* create_rule(token_t* name);

#endif /* _PARSER_H_ */

