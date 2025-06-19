/**
 * @file ast.c
 *
 * @brief Abstract Syntax Tree
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */

#include "ast.h"
#include "parser.h"
#include "pointer_list.h"
#include "alloc.h"
#include "scanner.h"
#include "trace.h"
#include "errors.h"

#ifdef USE_TRACE

static inline void trace_token(token_t* tok) {

    if(tok != NULL) {
        switch(tok->type) {
            case TERM_PRETEXT:
            case TERM_PRECODE:
            case TERM_POSTCODE:
            case TERM_ERROR:
            case TERM_LIST:
            case TERM_HEADER:
            case TERM_CODE_BLOCK:
                TRACE("terminal: %s", raw_string(tok->type_str));
                break;
            case TERM_TERMINAL_SYMBOL:
            case TERM_TERMINAL_KEYWORD:
            case TERM_TERMINAL_OPER:
            case TERM_NON_TERMINAL:
                TRACE("terminal: %s: %s", raw_string(tok->type_str), raw_string(tok->raw_str));
                break;
            default:
                FATAL("internal error: invalid node type in trace_token()");
        }
    }
}

#define TRACE_TOKEN(n) trace_token(n)

#define AST_ENTER \
    do { \
        ASSERT(node != NULL, "AST node cannot be NULL"); \
        ASSERT(node->type == AST_NTERM, "AST node is not a non-terminal node"); \
        ENTER; \
    } while(0)

#define AST_RETURN RETURN()

#else

#define TRACE_TOKEN(n)
#define AST_ENTER
#define AST_RETURN

#endif

extern int yylineno;

static void traverse_grammar(ast_grammar_t* node);
static void traverse_grammar_item(ast_grammar_item_t* node);
static void traverse_directive(ast_directive_t* node);
static void traverse_rule(ast_rule_t* node);
static void traverse_alternative(ast_alternative_t* node);
static void traverse_alternative_list(ast_alternative_list_t* node);
static void traverse_rule_element_list(ast_rule_element_list_t* node);
static void traverse_rule_element(ast_rule_element_t* node);
static void traverse_list_clause(ast_list_clause_t* node);


static void traverse_grammar(ast_grammar_t* node) {

    AST_ENTER;

    int mark = 0;
    ast_node_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        traverse_grammar_item((ast_grammar_item_t*)item);

    AST_RETURN;
}

static void traverse_grammar_item(ast_grammar_item_t* node) {

    AST_ENTER;

    if(node->node->type == NTERM_DIRECTIVE)
        traverse_directive((ast_directive_t*)node->node);
    else if(node->node->type == NTERM_RULE)
        traverse_rule((ast_rule_t*)node->node);
    else
        FATAL("unknown node type in traverse_grammar_item(): %d", node->node->type);

    AST_RETURN;
}

static void traverse_directive(ast_directive_t* node) {

    AST_ENTER;

    TRACE_TOKEN(node->dir_type);
    TRACE_TOKEN(node->code);

    AST_RETURN;
}

static void traverse_rule(ast_rule_t* node) {

    AST_ENTER;

    TRACE_TOKEN(node->nterm);
    traverse_alternative_list(node->alt_list);

    // could be NULL if there is no error execution clause
    TRACE_TOKEN(node->ecode);

    AST_RETURN;
}

static void traverse_alternative(ast_alternative_t* node) {

    AST_ENTER;

    traverse_rule_element_list(node->re_list);
    TRACE_TOKEN(node->code);

    AST_RETURN;
}

static void traverse_alternative_list(ast_alternative_list_t* node) {

    AST_ENTER;

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        traverse_alternative((ast_alternative_t*)item);

    AST_RETURN;
}

static void traverse_rule_element_list(ast_rule_element_list_t* node) {

    AST_ENTER;

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        traverse_rule_element((ast_rule_element_t*)item);

    AST_RETURN;
}

static void traverse_rule_element(ast_rule_element_t* node) {

    AST_ENTER;

    if(node->token != NULL) {
        TRACE_TOKEN(node->token);
    }
    else if(node->clause != NULL)
        traverse_list_clause(node->clause);
    else
        FATAL("invalid node in traverse_rule_element()");


    AST_RETURN;
}

static void traverse_list_clause(ast_list_clause_t* node) {

    AST_ENTER;

    traverse_rule_element_list(node->re_list);

    AST_RETURN;
}

static size_t nterm_size(ast_nterm_type_t type) {

    return (type == NTERM_ALTERNATIVE)? sizeof(ast_alternative_t) :
    (type == NTERM_ALTERNATIVE_LIST)? sizeof(ast_alternative_list_t) :
    (type == NTERM_DIRECTIVE)? sizeof(ast_directive_t) :
    (type == NTERM_GRAMMAR)? sizeof(ast_grammar_t) :
    (type == NTERM_GRAMMAR_ITEM)? sizeof(ast_grammar_item_t) :
    (type == NTERM_RULE)? sizeof(ast_rule_t) :
    (type == NTERM_RULE_ELEMENT)? sizeof(ast_rule_element_t) :
    (type == NTERM_RULE_ELEMENT_LIST)? sizeof(ast_rule_element_list_t) :
    (type == NTERM_LIST)? sizeof(ast_list_clause_t) : (size_t)-1;
}

/*
 * PUBLIC INTERFACE
 */

ast_node_t* create_ast_node(const char* name, ast_nterm_type_t type) {

    ast_node_t* node = _ALLOC(nterm_size(type));
    node->type = type;
    node->name = create_string(name);

    return node;
}

const char* nterm_to_str(ast_node_t* node) {

    return raw_string(node->name);
}

const char* nterm_type_to_str(ast_node_t* node) {

    return
        (node->type == NTERM_ALTERNATIVE)? "NTERM_ALTERNATIVE" :
        (node->type == NTERM_ALTERNATIVE_LIST)? "NTERM_ALTERNATIVE_LIST" :
        (node->type == NTERM_DIRECTIVE)? "NTERM_DIRECTIVE" :
        (node->type == NTERM_GRAMMAR)? "NTERM_GRAMMAR" :
        (node->type == NTERM_RULE)? "NTERM_RULE" :
        (node->type == NTERM_RULE_ELEMENT)? "NTERM_RULE_ELEMENT" :
        (node->type == NTERM_RULE_ELEMENT_LIST)? "NTERM_RULE_ELEMENT_LIST" :
        (node->type == NTERM_LIST)? "NTERM_LIST" : "UNKNOWN";
}

void traverse_ast(ast_grammar_t* node) {

    LOCAL_VERBOSITY(9);
    HEADER;
    AST_ENTER;

    traverse_grammar(root_node);

    AST_RETURN;

}

