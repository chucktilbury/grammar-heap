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

token_t* create_token(const char* name, int type) {

    token_t* tok = _ALLOC_TYPE(token_t);
    tok->raw_str = create_string(name);
    tok->line_no = yylineno;
    tok->type = type;

    switch(type) {
        case TERM_TERMINAL_KEYWORD: {
            string_t* tmp = create_string_fmt("TERM_%s", name);
            strip_char(tmp, '\'');
            upcase(tmp);
            tok->type_str = tmp;
        } break;

        case TERM_TERMINAL_OPER: {
            string_t* tmp = create_string_fmt("TERM_%s", name);
            strip_char(tmp, '\'');
            tok->type_str = convert(tmp);
            destroy_string(tmp);
        } break;

        case TERM_TERMINAL_SYMBOL: tok->type_str = create_string_fmt("TERM_%s", name); break;
        case TERM_CODE_BLOCK:      tok->type_str = create_string("TERM_CODE_BLOCK");   break;
        case TERM_NON_TERMINAL:    tok->type_str = create_string("TERM_NON_TERMINAL"); break;
        case TERM_LIST:            tok->type_str = create_string("TERM_LIST");         break;
        case TERM_HEADER:          tok->type_str = create_string("TERM_HEADER");       break;
        case TERM_PRETEXT:         tok->type_str = create_string("TERM_PRETEXT");      break;
        case TERM_PRECODE:         tok->type_str = create_string("TERM_PRECODE");      break;
        case TERM_POSTCODE:        tok->type_str = create_string("TERM_POSTCODE");     break;
        case TERM_ERROR:           tok->type_str = create_string("TERM_ERROR");        break;
        default: FATAL("unknown token type: %s", token_to_str(type));
    }

    return tok;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        destroy_string(tok->raw_str);
        destroy_string(tok->type_str);
        _FREE(tok);
    }
}

const char* tok_to_str(token_t* tok) {

    return raw_string(tok->raw_str);
}
