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
#include "errors.h"

#define USE_TRACE
#include "trace.h"

#ifdef USE_TRACE
#define TRACE_TOKEN(t) TRACE("token: \"%s\": %s", raw_string((t)->str), tok_to_str((t)->type))
#else
#define TRACE_TOKEN(t)
#endif

static void grammar(grammar_t* node);
static void grammar_list(grammar_list_t* node);
static void grammar_rule(grammar_rule_t* node);
static void rule_element_list(rule_element_list_t* node);
static void rule_element(rule_element_t* node);
static void or_function(or_function_t* node);
static void zero_or_more_function(zero_or_more_function_t* node);
static void zero_or_one_function(zero_or_one_function_t* node);
static void one_or_more_function(one_or_more_function_t* node);
static void grouping_function(grouping_function_t* node);

/*
 grammar
 : grammar_list
 ;
 */
static void grammar(grammar_t* node) {

    ENTER;

    grammar_list(node->grammar_list);

    RETURN();
}


/*
 grammar_list
 : non_terminal_rule  SEMI
 | grammar non_terminal_rule SEMI
 ;
 */
static void grammar_list(grammar_list_t* node) {

    ENTER;
    int mark = 0;
    grammar_rule_t* rule;

    while(NULL != (rule = (grammar_rule_t*)iterate_ast_node_list(node->list, &mark)))
        grammar_rule(rule);

    RETURN();
}


/*
grammar_rule
 : NON_TERMINAL grouping_function
 ;
 */
static void grammar_rule(grammar_rule_t* node) {

    ENTER;

    TRACE_TOKEN(node->NON_TERMINAL);
    grouping_function(node->grouping_function);

    RETURN();
}


/*
rule_element_list
 : rule_element
 | rule_element_list rule_element
 ;
 */
static void rule_element_list(rule_element_list_t* node) {

    ENTER;
    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);

    RETURN();
}


/*
rule_element
 : NON_TERMINAL
 | TERMINAL_NAME
 | TERMINAL_OPER
 | TERMINAL_SYMBOL
 | or_function
 | zero_or_more_function
 | zero_or_one_function
 | one_or_more_function
 | grouping_function
 ;
 */
static void rule_element(rule_element_t* node) {

    ENTER;

    if(node->token != NULL) {
        TRACE_TOKEN(node->token);
        switch(node->token->type) {
            case TERMINAL_KEYWORD:
                break;
            case TERMINAL_OPER:
                break;
            case TERMINAL_SYMBOL:
                break;
            case NON_TERMINAL:
                break;
            default:
                FATAL("unknown terminal type: %s", tok_to_str(node->token->type));
        }
    }
    else if(node->nterm != NULL) {
        switch(node->nterm->type) {
            case AST_OR_FUNCTION:
                or_function((or_function_t*)node->nterm);
                break;
            case AST_ZERO_OR_MORE_FUNCTION:
                zero_or_more_function((zero_or_more_function_t*)node->nterm);
                break;
            case AST_ZERO_OR_ONE_FUNCTION:
                zero_or_one_function((zero_or_one_function_t*)node->nterm);
                break;
            case AST_ONE_OR_MORE_FUNCTION:
                one_or_more_function((one_or_more_function_t*)node->nterm);
                break;
            case AST_GROUPING_FUNCTION:
                grouping_function((grouping_function_t*)node->nterm);
                break;
            default:
                FATAL("unknown non-terminal type: %s", nterm_to_str(node->nterm->type));
        }
    }
    else
        FATAL("invalid rule element");

    RETURN();
}

/*
or_function
 : rule_element PIPE rule_element
 ;
 */
static void or_function(or_function_t* node) {

    ENTER;

    rule_element(node->left);
    rule_element(node->right);

    RETURN();
}

/*
zero_or_more_function
 : rule_element QUESTION
 ;
 */
static void zero_or_more_function(zero_or_more_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}

/*
zero_or_one_function
 : rule_element STAR
 ;
 */
static void zero_or_one_function(zero_or_one_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}


/*
one_or_more_function
 : rule_element PLUS
 ;
 */
static void one_or_more_function(one_or_more_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}

/*
grouping_function
 : OPAREN rule_element_list CPAREN
 ;
 */
static void grouping_function(grouping_function_t* node) {

    ENTER;

    rule_element_list(node->rule_element_list);

    RETURN();
}

#include "alloc.h"

static size_t get_node_size(ast_type_t type) {

    return (type == AST_GRAMMAR)                ? sizeof(grammar_t) :
            (type == AST_GRAMMAR_LIST)          ? sizeof(grammar_list_t) :
            (type == AST_GRAMMAR_RULE)          ? sizeof(grammar_rule_t) :
            (type == AST_RULE_ELEMENT_LIST)     ? sizeof(rule_element_list_t) :
            (type == AST_RULE_ELEMENT)          ? sizeof(rule_element_t) :
            (type == AST_OR_FUNCTION)           ? sizeof(or_function_t) :
            (type == AST_ZERO_OR_MORE_FUNCTION) ? sizeof(zero_or_more_function_t) :
            (type == AST_ZERO_OR_ONE_FUNCTION)  ? sizeof(zero_or_one_function_t) :
            (type == AST_ONE_OR_MORE_FUNCTION)  ? sizeof(one_or_more_function_t) :
            (type == AST_GROUPING_FUNCTION)     ? sizeof(grouping_function_t) :
                                                  (size_t)-1;
}

ast_node_t* create_ast_node(ast_type_t type) {

    ast_node_t* node = _ALLOC(get_node_size(type));
    node->type = type;

    return node;
}

void traverse_ast(ast_node_t* node) {

    grammar((grammar_t*)node);
}

ast_node_list_t* create_ast_node_list(void) {

    return (ast_node_list_t*)create_ptr_list();
}

void append_ast_node_list(ast_node_list_t* lst, ast_node_t* ptr) {

    append_ptr_list((pointer_list_t*)lst, (void*)ptr);
}

ast_node_t* iterate_ast_node_list(ast_node_list_t* lst, int* post) {

    return (ast_node_t*)iterate_ptr_list((pointer_list_t*)lst, post);
}

int len_ast_node_list(ast_node_list_t* lst) {

    return len_ptr_list((pointer_list_t*)lst);
}

const char* nterm_to_str(ast_type_t type) {

    return (type == AST_GRAMMAR)                ? "AST_GRAMMAR" :
            (type == AST_GRAMMAR_LIST)          ? "AST_GRAMMAR_LIST" :
            (type == AST_GRAMMAR_RULE)          ? "AST_GRAMMAR_RULE" :
            (type == AST_RULE_ELEMENT_LIST)     ? "AST_RULE_ELEMENT_LIST" :
            (type == AST_RULE_ELEMENT)          ? "AST_RULE_ELEMENT" :
            (type == AST_OR_FUNCTION)           ? "AST_OR_FUNCTION" :
            (type == AST_ZERO_OR_MORE_FUNCTION) ? "AST_ZERO_OR_MORE_FUNCTION" :
            (type == AST_ZERO_OR_ONE_FUNCTION)  ? "AST_ZERO_OR_ONE_FUNCTION" :
            (type == AST_ONE_OR_MORE_FUNCTION)  ? "AST_ONE_OR_MORE_FUNCTION" :
            (type == AST_GROUPING_FUNCTION)     ? "AST_GROUPING_FUNCTION" :
                                                  "UNKNOWN";
}
