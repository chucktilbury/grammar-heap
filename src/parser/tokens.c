/**
 * @file tokens.c
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */

#include "alloc.h"
#include "tokens.h"
#include "parser.h"
#include "errors.h"
extern int yylineno;

token_t* create_token(const char* str, int type) {

    token_t* tok = _ALLOC_TYPE(token_t);
    tok->str = create_string(str);
    tok->line_no = yylineno;
    tok->type = type;

    switch(type) {
        case TERMINAL_SYMBOL: {
            tok->ptype = create_string_fmt("TOK_%s", str);
        } break;

        case TERMINAL_KEYWORD: {
            string_t* tmp = create_string_fmt("TOK_%s", str);
            strip_char(tmp, '\'');
            upcase(tmp);
            tok->ptype = tmp;
        } break;

        case TERMINAL_OPER: {
            string_t* tmp = create_string_fmt("TOK_%s", str);
            strip_char(tmp, '\'');
            tok->ptype = convert(tmp);
            destroy_string(tmp);
        } break;

        case CODE_BLOCK:
        case NON_TERMINAL:
            // do nothing
            break;

        default:
            FATAL("unknown token type: %s", token_to_str(type));
    }

    return tok;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        destroy_string(tok->str);
        _FREE(tok);
    }
}

const char* tok_to_str(int type) {

    return (type == PLUS)              ? "PLUS" :
            (type == STAR)             ? "STAR" :
            (type == QUESTION)         ? "QUESTION" :
            (type == PIPE)             ? "PIPE" :
            (type == OPAREN)           ? "OPAREN" :
            (type == CPAREN)           ? "CPAREN" :
            (type == NON_TERMINAL)     ? "NON_TERMINAL" :
            (type == TERMINAL_SYMBOL)  ? "TERMINAL_SYMBOL" :
            (type == TERMINAL_KEYWORD) ? "TERMINAL_KEYWORD" :
            (type == TERMINAL_OPER)    ? "TERMINAL_OPER" :
            (type == CODE_BLOCK)    ? "CODE_BLOCK" :
                                         "UNKNOWN";
}
