%{
#include <stdio.h>
#include <stdint.h>

#include "tokens.h"
#include "string_buffer.h"
#include "pointer_list.h"
#include "alloc.h"

int yylex(void);
void yyerror(const char*);
extern int yylineno;
int errors = 0;

typedef struct {
    string_t* name;
    string_t* expr;
} rule_t;

rule_t* rule;
pointer_list_t* rlist;

void init_rule(string_t* name) {
    clear_string(rule->name);
    clear_string(rule->expr);
    append_string_str(rule->name, name);
}

void add_rule(string_t* str) {
    append_string_str(rule->expr, str);
}

void print_rule(rule_t* rule) {
    printf("%s: %s;\n", rule->name->buffer, rule->expr->buffer);
}


%}

// support the typedefs in the %union.
%code requires {
#include <stdint.h>
#include <stdbool.h>
#include "ast.h"
#include "tokens.h"
#include "pointer_list.h"
}

// this goes at the bottom of the generated header file.
%code provides {
const char* token_to_str(int);
void init_parser(void);
extern grammar_t* root_node;
extern int errors;
}

%union {
    token_t* token;
    rule_t* rule;
};

%token PLUS STAR QUESTION PIPE OPAREN CPAREN COLON SEMICOLON
%token PRETEXT PRECODE POSTCODE POSTTEXT
%token PROVIDES REQUIRES TERM_DEF NTERM_DEF
%token<token> TERMINAL_SYMBOL TERMINAL_KEYWORD TERMINAL_OPER
%token<token> CODE_BLOCK NON_TERMINAL

%define parse.lac full
%define parse.error detailed
%locations
%verbose
    //%output "parser.c"
    //%defines

%type <rule> grammar_rule expression primary primary_list

%left PIPE
%left QUESTION STAR PLUS

%%

grammar
    : grammar_list {
        printf("complete the grammar\n");
    }
    ;

grammar_list
    : grammar_rule  {
        printf("create the grammar rule list\n");
    }
    | grammar_list grammar_rule {
        printf("add to the grammar rule list\n");
    }
    ;

directive
    : PRETEXT CODE_BLOCK {
        printf("create PRETEXT directive\n");
    }
    | POSTTEXT CODE_BLOCK {
        printf("create POSTTEXT directive\n");
    }
    | PRECODE CODE_BLOCK {
        printf("create PRECODE directive\n");
    }
    | POSTCODE CODE_BLOCK {
        printf("create POSTCODE directive\n");
    }
    | PROVIDES CODE_BLOCK {
        printf("create PROVIDES directive\n");
    }
    | REQUIRES CODE_BLOCK {
        printf("create REQUIRES directive\n");
    }
    | TERM_DEF CODE_BLOCK {
        printf("create TERM_DEF directive\n");
    }
    | NTERM_DEF CODE_BLOCK {
        printf("create NTERM_DEF directive\n");
    }
    ;

grammar_rule
    : NON_TERMINAL COLON {
        $$ = create_rule($1)
    } expression SEMICOLON {
        printf("complete grammar rule\n");
        print_rule($4);
    }
    | directive {
        printf("complete directive\n");
    }
    ;

primary
    : NON_TERMINAL {
        printf("non-terminal: %s\n", $1->str->buffer);
        $$ = create_string($1->str->buffer);
    }
    | TERMINAL_KEYWORD {
        printf("terminal: %s\n", $1->ptype->buffer);
        $$ = create_string($1->str->buffer);
    }
    | TERMINAL_OPER {
        printf("terminal: %s\n", $1->ptype->buffer);
        $$ = create_string($1->str->buffer);
    }
    | TERMINAL_SYMBOL {
        printf("terminal: %s\n", $1->ptype->buffer);
        $$ = create_string($1->str->buffer);
    }
    ;

primary_list
    : primary {
        printf("create primary list: %s\n", $1);
        $$ = create_string($1->buffer);
    }
    | primary_list primary {
        printf("concatanate primary list: %s\n", $2);
        append_string($1, $2->buffer);
    }
    ;

expression
    : primary_list {
        $$ = $1;
    }
    | expression STAR {
        printf("%s*\n", $1);
        $$ = $1;
    }
    | expression QUESTION {
        printf("%s?\n", $1);
        $$ = $1;
    }
    | expression PLUS {
        printf("%s+\n", $1);
        $$ = $1;
    }
    | expression PIPE expression {
        printf("%s|%s\n", $1, $3);
        $$ = $1;
    }
    | OPAREN expression CPAREN {
        printf("(%s)\n", $2);
        $$ = $2;
    }
    | OPAREN expression CPAREN CODE_BLOCK {
        printf("(%s){CODE}\n", $2);
        $$ = $2;
    }
    ;

%%

#include <string.h>
#include <errno.h>
#include "cmdline.h"
#include "trace.h"
#include "errors.h"
#include "fileio.h"

extern FILE* yyin;

void yyerror(const char* s) {

    fprintf(stderr, "%d: %s\n", yylineno, s);
    errors++;
}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}

void init_parser(void) {

    if(in_cmd_list("dump", "parser"))
        LOCAL_VERBOSITY(0);
    else
        LOCAL_VERBOSITY(19);

    TRACE_HEADER;

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
