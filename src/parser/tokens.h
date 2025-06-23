/**
 * @file tokens.h
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */
#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "string_buffer.h"
#define TERM_START  200

typedef enum {
    TERM_PRETEXT = TERM_START,
    TERM_POSTTEXT,
    TERM_PRECODE,
    TERM_POSTCODE,
    TERM_ERROR,
    TERM_LIST,
    TERM_HEADER,
    TERM_TERM_DEF,
    TERM_NTERM_DEF,
    TERM_TERMINAL_SYMBOL,
    TERM_TERMINAL_KEYWORD,
    TERM_TERMINAL_OPER,
    TERM_CODE_BLOCK,
    TERM_NON_TERMINAL,
} ast_term_type_t;

typedef struct {
    int type;
    string_t* raw_str;
    string_t* type_str;
    int line_no;
} token_t;

token_t* create_token(const char* str, int type);
void destroy_token(token_t* tok);
const char* tok_to_str(token_t* tok);

#endif /* _TOKENS_H_ */
