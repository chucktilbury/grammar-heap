
#include "alloc.h"
#include "tokens.h"
#include "parser.h"
#include "errors.h"
#include "cmdline.h"
#include "fileio.h"
#include "scanner.h"

static token_t* token = NULL;

void init_scanner(void) {

    const char* fname = find_file(raw_string(get_cmd_opt("files")), ".g");
    if(fname != NULL) {
        yyin = fopen(fname, "r");
        if(yyin == NULL) {
            fprintf(stderr, "cannot open input file \"%s\": %s\n", fname, strerror(errno));
            cmdline_help();
        }
    }
    else
        FATAL("internal error in %s: parse command line failed", __func__);
}

token_t* create_token(const char* str, token_type_t type) {

    token_t* tok = _ALLOC_TYPE(token_t);
    tok->str = create_string(str);
    tok->line_no = yylineno;
    tok->type = type;

    switch(type) {
        case TERMINAL_SYMBOL: {
            tok->ptype = create_string_fmt("TERM_%s", str);
        } break;

        case TERMINAL_KEYWORD: {
            string_t* tmp = create_string_fmt("TERM_%s", str);
            strip_char(tmp, '\'');
            upcase(tmp);
            tok->ptype = tmp;
        } break;

        case TERMINAL_OPER: {
            string_t* tmp = create_string_fmt("TERM_%s", str);
            strip_char(tmp, '\'');
            tok->ptype = convert(tmp);
            destroy_string(tmp);
        } break;

        default:
            tok->ptype = create_string(tok_to_str(type));
            break;
    }

    return tok;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        destroy_string(tok->str);
        destroy_string(tok->ptype);
        _FREE(tok);
    }
}

token_t* get_token(void) {

    //fprintf(stderr, "get: \"%s\" \"%s\"\n", token->str->buffer, token->ptype->buffer);
    return token;
}

void set_token(token_t* tok) {

    token = tok;
}

token_t* copy_token(token_t* tok) {

    token_t* ptr = _ALLOC_TYPE(token_t);
    ptr->line_no = tok->line_no;
    ptr->type = tok->type;
    ptr->ptype = copy_string(tok->ptype);
    ptr->str = copy_string(tok->str);

    return ptr;
}

token_t* consume_token(void) {

    //if(token!= NULL) fprintf(stderr, "consume: \"%s\" \"%s\"\n", token->str->buffer, token->ptype->buffer);
    destroy_token(token);
    if(yylex() != 0)
        return token;
    else {
        token = NULL;
        return NULL;
    }
}

const char* tok_to_str(token_type_t type) {

    return (type == PRETEXT)? "PRETEXT":
        (type == POSTTEXT)? "POSTTEXT":
        (type == PRECODE)? "PRECODE":
        (type == POSTCODE)? "POSTCODE":
        (type == TERM_DEF)? "TERM_DEF":
        (type == NTERM_DEF)? "NTERM_DEF":
        (type == PROVIDES)? "PROVIDES":
        (type == REQUIRES)? "REQUIRES":
        (type == PLUS)? "PLUS":
        (type == STAR)? "STAR":
        (type == QUESTION)? "QUESTION":
        (type == PIPE)? "PIPE":
        (type == OPAREN)? "OPAREN":
        (type == CPAREN)? "CPAREN":
        (type == COLON)? "COLON":
        (type == SEMICOLON)? "SEMICOLON":
        (type == CODE_BLOCK)? "CODE_BLOCK":
        (type == NON_TERMINAL)? "NON_TERMINAL":
        (type == TERMINAL_SYMBOL)? "TERMINAL_SYMBOL":
        (type == TERMINAL_KEYWORD)? "TERMINAL_KEYWORD":
        (type == TERMINAL_OPER)? "TERMINAL_OPER":"UNKNOWN";
}
