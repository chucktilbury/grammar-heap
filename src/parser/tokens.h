
#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "string_buffer.h"

typedef enum {
    PRETEXT,
    POSTTEXT,
    PRECODE,
    POSTCODE,
    TERM_DEF,
    NTERM_DEF,
    PROVIDES,
    REQUIRES,
    PLUS,
    STAR,
    QUESTION,
    PIPE,
    OPAREN,
    CPAREN,
    COLON,
    SEMICOLON,
    CODE_BLOCK,
    NON_TERMINAL,
    TERMINAL_SYMBOL,
    TERMINAL_KEYWORD,
    TERMINAL_OPER,
    CATENATE, // created by parser
} token_type_t;

typedef struct {
    string_t* str;
    string_t* ptype;
    token_type_t type;
    int line_no;
} token_t;

void init_scanner(void);
token_t* create_token(const char* str, token_type_t type);
void destroy_token(token_t* tok);
token_t* get_token(void);
void set_token(token_t* tok);
token_t* copy_token(token_t* tok);
token_t* consume_token(void);
const char* tok_to_str(token_type_t type);

#endif /* _TOKENS_H_ */
