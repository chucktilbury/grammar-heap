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

#include "pointer_list.h"
#include "string_buffer.h"
#include "tokens.h"

#define NTERM_START 100

typedef enum {
    NTERM_ALTERNATIVE = NTERM_START,
    NTERM_ALTERNATIVE_LIST,
    NTERM_DIRECTIVE,
    NTERM_GRAMMAR,
    NTERM_GRAMMAR_ITEM,
    NTERM_RULE,
    NTERM_RULE_ELEMENT,
    NTERM_RULE_ELEMENT_LIST,
    NTERM_LIST,
} ast_nterm_type_t;

typedef struct {
    ast_nterm_type_t type;
    string_t* name;
} ast_node_t;

typedef struct _ast_grammar_t_ {
    ast_node_t type;
    pointer_list_t* list;
} ast_grammar_t;

typedef struct _ast_grammar_item_t_ {
    ast_node_t type;
    ast_node_t* node;
} ast_grammar_item_t;

typedef struct _ast_directive_t_ {
    ast_node_t type;
    token_t* dir_type;
    token_t* code;
} ast_directive_t;

typedef struct _ast_alternative_list_t_ {
    ast_node_t type;
    pointer_list_t* list;
} ast_alternative_list_t;

typedef struct _ast_rule_t_ {
    ast_node_t type;
    token_t* nterm;
    struct _ast_alternative_list_t_* alt_list;
    token_t* ecode;
} ast_rule_t;

typedef struct _ast_alternative_t_ {
    ast_node_t type;
    struct _ast_rule_element_list_t_* re_list;
    token_t* code;
} ast_alternative_t;

typedef struct _ast_rule_element_list_t_ {
    ast_node_t type;
    pointer_list_t* list;
} ast_rule_element_list_t;

typedef struct _ast_rule_element_t_ {
    ast_node_t type;
    token_t* token;
    struct _ast_list_clause_t_* clause;
} ast_rule_element_t;

typedef struct _ast_list_clause_t_ {
    ast_node_t type;
    struct _ast_rule_element_list_t_* re_list;
} ast_list_clause_t;

ast_node_t* create_ast_node(const char* name, ast_nterm_type_t type);
void traverse_ast(void);
const char* nterm_to_str(ast_node_t* node);
const char* nterm_type_to_str(ast_node_t* node);

#endif /* _AST_H_ */
