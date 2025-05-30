%{
#include <stdio.h>
#include <stdint.h>

#include "tokens.h"
#include "ast.h"

int yylex(void);
void yyerror(const char*);
extern int yylineno;
grammar_t* root_node;

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
extern grammar_t* root_node;
}

%union {
    token_t* token;
    grammar_t* grammar;
    grammar_list_t* grammar_list;
    grammar_rule_t* grammar_rule;
    rule_element_list_t* rule_element_list;
    rule_element_t* rule_element;
    or_function_t* or_function;
    zero_or_more_function_t* zero_or_more_function;
    zero_or_one_function_t* zero_or_one_function;
    one_or_more_function_t* one_or_more_function;
    grouping_function_t* grouping_function;
};

%token PLUS STAR QUESTION PIPE OPAREN CPAREN
%token<token> NON_TERMINAL TERMINAL_SYMBOL TERMINAL_KEYWORD TERMINAL_OPER

%type <grammar> grammar
%type <grammar_list> grammar_list
%type <grammar_rule> grammar_rule
%type <rule_element_list> rule_element_list
%type <rule_element> rule_element
%type <or_function> or_function
%type <zero_or_more_function> zero_or_more_function
%type <zero_or_one_function> zero_or_one_function
%type <one_or_more_function> one_or_more_function
%type <grouping_function> grouping_function

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
    : grammar_list {
        root_node = $$ = (grammar_t*)create_ast_node(AST_GRAMMAR);
        $$->grammar_list = $1;
    }
    ;

grammar_list
    : grammar_rule  {
        $$ = (grammar_list_t*)create_ast_node(AST_GRAMMAR_LIST);
        $$->list = create_ast_node_list();
        append_ast_node_list($$->list, (ast_node_t*)$1);
    }
    | grammar_list grammar_rule {
        append_ast_node_list($$->list, (ast_node_t*)$2);
    }
    ;

grammar_rule
    : NON_TERMINAL grouping_function {
        $$ = (grammar_rule_t*)create_ast_node(AST_GRAMMAR_RULE);
        $$->NON_TERMINAL = $1;
        $$->grouping_function = $2;
    }
    ;

rule_element_list
    : rule_element {
        $$ = (rule_element_list_t*)create_ast_node(AST_RULE_ELEMENT_LIST);
        $$->list = create_ast_node_list();
        append_ast_node_list($$->list, (ast_node_t*)$1);
    }
    | rule_element_list rule_element {
        append_ast_node_list($$->list, (ast_node_t*)$2);
    }
    ;

rule_element
    : NON_TERMINAL {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->token = $1;
    }
    | TERMINAL_KEYWORD {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        strip_quotes($1->str);
        $$->token = $1;
    }
    | TERMINAL_OPER {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        strip_quotes($1->str);
        $$->token = $1;
    }
    | TERMINAL_SYMBOL {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->token = $1;
    }
    | or_function {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->nterm = (ast_node_t*)$1;
    }
    | zero_or_more_function {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->nterm = (ast_node_t*)$1;
    }
    | zero_or_one_function {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->nterm = (ast_node_t*)$1;
    }
    | one_or_more_function {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->nterm = (ast_node_t*)$1;
    }
    | grouping_function {
        $$ = (rule_element_t*)create_ast_node(AST_RULE_ELEMENT);
        $$->nterm = (ast_node_t*)$1;
    }
    ;

zero_or_more_function
    : rule_element STAR {
        $$ = (zero_or_more_function_t*)create_ast_node(AST_ZERO_OR_MORE_FUNCTION);
        $$->rule_element = $1;
    }
    ;

zero_or_one_function
    : rule_element QUESTION {
        $$ = (zero_or_one_function_t*)create_ast_node(AST_ZERO_OR_ONE_FUNCTION);
        $$->rule_element = $1;
    }
    ;

one_or_more_function
    : rule_element PLUS {
        $$ = (one_or_more_function_t*)create_ast_node(AST_ONE_OR_MORE_FUNCTION);
        $$->rule_element = $1;
    }
    ;

or_function
    : rule_element PIPE rule_element {
        $$ = (or_function_t*)create_ast_node(AST_OR_FUNCTION);
        $$->left = $1;
        $$->right = $3;
    }
    ;

grouping_function
    : OPAREN rule_element_list CPAREN {
        $$ = (grouping_function_t*)create_ast_node(AST_GROUPING_FUNCTION);
        $$->rule_element_list = $2;
    }
    ;

%%

void yyerror(const char* s) {

    fprintf(stderr, "%d: %s\n", yylineno, s);
}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}

