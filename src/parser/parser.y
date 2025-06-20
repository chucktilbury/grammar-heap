%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tokens.h"
#include "ast.h"
#include "trace.h"
#include "cmdline.h"
#include "errors.h"
#include "master_list.h"

int yylex(void);
void yyerror(const char*);
extern int yylineno;
extern FILE *yyin, *yyout;

int errors = 0;
int header_code_seen = 0;
int pre_text_seen = 0;
int pre_code_seen = 0;
int post_code_seen = 0;

%}

// support the typedefs in the %union.
%code requires {
#include "ast.h"
#include "tokens.h"
#include "pointer_list.h"
}

// this goes at the bottom of the generated header file.
%code provides {
const char* token_to_str(int);
void init_parser(void);
extern int errors;
}

%union {
    token_t* token;
    ast_grammar_t* grammar;
    ast_grammar_item_t* grammar_item;
    ast_directive_t* directive;
    ast_alternative_list_t* alternative_list;
    ast_rule_t* rule;
    ast_alternative_t* alternative;
    ast_rule_element_list_t* rule_element_list;
    ast_rule_element_t* rule_element;
    ast_list_clause_t* list_clause;
};

%token<token> PRETEXT PRECODE POSTCODE ERROR LIST HEADERCODE
%token<token> TERMINAL_SYMBOL TERMINAL_KEYWORD TERMINAL_OPER
%token<token> CODE_BLOCK NON_TERMINAL

%type<grammar> grammar
%type<grammar_item> grammar_item
%type<directive> directive
%type<alternative_list> alternative_list
%type<rule> rule
%type<alternative> alternative
%type<rule_element_list> rule_element_list
%type<rule_element> rule_element
%type<list_clause> list_clause

%define parse.lac full
%define parse.error detailed
%locations
%verbose
    //%output "parser.c"
    //%defines

%left PIPE
%left QUESTION STAR PLUS

%%

grammar
    : grammar_item  {
            TRACE("create the grammar");
            $$ = (ast_grammar_t*)create_ast_node("grammar", NTERM_GRAMMAR);
            set_master_list_root_node($$);
            $$->list = create_ptr_list();
            append_ptr_list($$->list, $1);
        }
    | grammar grammar_item {
            TRACE("add grammar rule");
            append_ptr_list($1->list, $2);
        }
    ;

grammar_item
    : rule {
            TRACE("rule grammar item");
            $$ = (ast_grammar_item_t*)create_ast_node("grammar_item", NTERM_GRAMMAR_ITEM);
            $$->node = (ast_node_t*)$1;
        }
    | directive {
            TRACE("directive grammar item");
            $$ = (ast_grammar_item_t*)create_ast_node("grammar_item", NTERM_GRAMMAR_ITEM);
            $$->node = (ast_node_t*)$1;
        }
    ;

directive
    : PRETEXT CODE_BLOCK {
            if(pre_text_seen)
                yyerror("only one %pretext directive is allowed in module");
            else
                pre_text_seen++;

            TRACE("PRETEXT directive");
            $$ = (ast_directive_t*)create_ast_node("directive", NTERM_DIRECTIVE);
            $$->dir_type = $1;
            $$->code = $2;
        }
    | PRECODE CODE_BLOCK {
            if(pre_code_seen)
                yyerror("only one %precode directive is allowed in module");
            else
                pre_code_seen++;

            TRACE("PRECODE directive");
            $$ = (ast_directive_t*)create_ast_node("directive", NTERM_DIRECTIVE);
            $$->dir_type = $1;
            $$->code = $2;
        }
    | POSTCODE CODE_BLOCK {
            if(post_code_seen)
                yyerror("only one %postcode directive is allowed in module");
            else
                post_code_seen++;

            TRACE("POSTCODE directive");
            $$ = (ast_directive_t*)create_ast_node("directive", NTERM_DIRECTIVE);
            $$->dir_type = $1;
            $$->code = $2;
        }
    | HEADERCODE CODE_BLOCK {
            if(header_code_seen)
                yyerror("only one %header directive is allowed in module");
            else
                header_code_seen++;

            TRACE("HEADER directive");
            $$ = (ast_directive_t*)create_ast_node("directive", NTERM_DIRECTIVE);
            $$->dir_type = $1;
            $$->code = $2;
        }
    ;

rule
    : NON_TERMINAL ':' alternative_list ';' {
            TRACE("rule without error block");
            $$ = (ast_rule_t*)create_ast_node("rule", NTERM_RULE);
            $$->nterm = $1;
            $$->alt_list = $3;
        }
    ;

alternative_list
    : alternative {
            TRACE("create alternative");
            $$ = (ast_alternative_list_t*)create_ast_node("alternative_list", NTERM_ALTERNATIVE_LIST);
            $$->list = create_ptr_list();
            append_ptr_list($$->list, $1);
        }
    | alternative_list '|' alternative {
            TRACE("add alternative");
            append_ptr_list($1->list, $3);
        }
    ;

alternative
    : rule_element_list CODE_BLOCK {
            TRACE("normal alternative");
            $$ = (ast_alternative_t*)create_ast_node("alternative", NTERM_ALTERNATIVE);
            $$->re_list = $1;
            $$->code = $2;
        }
    | ERROR CODE_BLOCK {
            TRACE("error alternative");
            $$ = (ast_alternative_t*)create_ast_node("alternative", NTERM_ALTERNATIVE);
            $$->re_list = NULL;
            $$->code = $2;
        }
    ;

rule_element_list
    : rule_element {
            TRACE("create rule element");
            $$ = (ast_rule_element_list_t*)create_ast_node("rule_element_list", NTERM_RULE_ELEMENT_LIST);
            $$->list = create_ptr_list();
            append_ptr_list($$->list, $1);
        }
    | rule_element_list rule_element {
            TRACE("add rule element");
            append_ptr_list($1->list, $2);
        }
    ;

rule_element
    : TERMINAL_SYMBOL {
            TRACE("rule_element: TERMINAL_SYMBOL");
            $$ = (ast_rule_element_t*)create_ast_node("rule_element", NTERM_RULE_ELEMENT);
            $$->token = $1;
            $$->clause = NULL;
        }
    | TERMINAL_KEYWORD {
            TRACE("rule_element: TERMINAL_KEYWORD");
            $$ = (ast_rule_element_t*)create_ast_node("rule_element", NTERM_RULE_ELEMENT);
            $$->token = $1;
            $$->clause = NULL;
        }
    | TERMINAL_OPER {
            TRACE("rule_element: TERMINAL_OPER");
            $$ = (ast_rule_element_t*)create_ast_node("rule_element", NTERM_RULE_ELEMENT);
            $$->token = $1;
            $$->clause = NULL;
        }
    | NON_TERMINAL {
            TRACE("rule_element: NON_TERMINAL");
            $$ = (ast_rule_element_t*)create_ast_node("rule_element", NTERM_RULE_ELEMENT);
            $$->token = $1;
            $$->clause = NULL;
        }
    | list_clause {
            TRACE("rule_element: list_clause");
            $$ = (ast_rule_element_t*)create_ast_node("rule_element", NTERM_RULE_ELEMENT);
            $$->clause = $1;
            $$->token = NULL;
        }
    ;

list_clause
    : LIST '(' rule_element_list ')' {
            $$ = (ast_list_clause_t*)create_ast_node("list_clause", NTERM_LIST);
            $$->re_list = $3;
        }
    ;

%%

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

    const char* fname = raw_string(get_cmd_opt("files"));
    if(fname != NULL) {
        yyin = fopen(fname, "r");
        if(yyin == NULL) {
            fprintf(stderr, "cannot open input file \"%s\": %s\n", fname, strerror(errno));
            cmdline_help();
        }
    }
    else
        FATAL("internal error in %s: command line failed", __func__);
}
