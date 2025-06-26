/**
 * @file ast.h
 *
 * @brief Abstract Syntax Tree
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */
#ifndef _AST_H_
#define _AST_H_

#include "string_list.h"
#include "pointer_list.h"
#include "tokens.h"

typedef enum {
    NTERM_GRAMMAR,
    NTERM_GRAMMAR_LIST,
    NTERM_GRAMMAR_RULE,
    NTERM_RULE_ELEMENT_LIST,
    NTERM_RULE_ELEMENT,
    NTERM_OR_FUNCTION,
    NTERM_ZERO_OR_MORE_FUNCTION,
    NTERM_ZERO_OR_ONE_FUNCTION,
    NTERM_ONE_OR_MORE_FUNCTION,
    NTERM_GROUPING_FUNCTION,
    NTERM_DIRECTIVE,
    NTERM_INLINE_CODE,
} ast_type_t;

typedef struct
{
    ast_type_t type;
} ast_node_t;

typedef pointer_list_t ast_node_list_t;

/*
grammar
    : grammar_list
    ;
 */
typedef struct _grammar_t_ {
    ast_node_t node;
    struct _grammar_list_t_* grammar_list;
} grammar_t;

/*
grammar_list
    : grammar_rule
    | grammar grammar_rule
    ;
 */
typedef struct _grammar_list_t_ {
    ast_node_t node;
    ast_node_list_t* list;
} grammar_list_t;

typedef struct _directive_t_ {
    ast_node_t node;
    int type;
    token_t* code;
} directive_t;

/*
 grammar_rule
    : NON_TERMINAL grouping_function
    ;
 */
typedef struct _grammar_rule_t_ {
    ast_node_t node;
    token_t* NON_TERMINAL;
    struct _grouping_function_t_* grouping_function;
    struct _directive_t_* directive;
} grammar_rule_t;

/*
rule_element_list
    : rule_element
    | rule_element_list rule_element
    ;
 */
typedef struct _rule_element_list_t_ {
    ast_node_t node;
    ast_node_list_t* list;
} rule_element_list_t;

/*
 rule_element
    : NON_TERMINAL
    | TERMINAL_KEYWORD
    | TERMINAL_OPER
    | TERMINAL_SYMBOL
    | or_function
    | zero_or_more_function
    | zero_or_one_function
    | one_or_more_function
    | grouping_function
    ;
 */
typedef struct _rule_element_t_ {
    ast_node_t node;
    token_t* token;
    ast_node_t* nterm;
} rule_element_t;

/*
or_function
    : rule_element PIPE
    ;
 */
typedef struct _or_function_t_ {
    ast_node_t node;
    struct _rule_element_t_* left;
    struct _rule_element_t_* right;
    int state;
    int level;
    string_list_t* impl;
} or_function_t;

/*
zero_or_more_function
    : rule_element QUESTION
    ;
 */
typedef struct _zero_or_more_function_t_ {
    ast_node_t node;
    struct _rule_element_t_* rule_element;
    int state;
    int level;
    string_list_t* impl;
} zero_or_more_function_t;

/*
zero_or_one_function
    : rule_element STAR
    ;
 */
typedef struct _zero_or_one_function_t_ {
    ast_node_t node;
    struct _rule_element_t_* rule_element;
    int state;
    int level;
    string_list_t* impl;
} zero_or_one_function_t;

/*
one_or_more_function
    : rule_element PLUS
    ;
 */
typedef struct _one_or_more_function_t_ {
    ast_node_t node;
    struct _rule_element_t_* rule_element;
    int state;
    int level;
    string_list_t* impl;
} one_or_more_function_t;

/*
grouping_function
    : OPAREN rule_element_list CPAREN
    ;
 */
typedef struct _grouping_function_t_ {
    ast_node_t node;
    struct _rule_element_list_t_* rule_element_list;
    int state;
    int level;
    string_list_t* impl;
} grouping_function_t;

typedef struct _inline_code_t_ {
    ast_node_t node;
    struct _grouping_function_t_* group;
    token_t* code;
} inline_code_t;

ast_node_t* create_ast_node(ast_type_t type);
void traverse_ast(void);

ast_node_list_t* create_ast_node_list(void);
void append_ast_node_list(ast_node_list_t* lst, ast_node_t* ptr);
ast_node_t* iterate_ast_node_list(ast_node_list_t* lst, int* post);
int len_ast_node_list(ast_node_list_t* lst);
const char* nterm_to_str(ast_type_t type);

#endif /* _AST_H_ */
