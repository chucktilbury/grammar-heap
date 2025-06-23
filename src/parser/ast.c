/**
 * @file ast.c
 *
 * @brief Abstract Syntax Tree
 *
 * This is the default tree traverse function set. It is intended as a
 * template for other AST passes. If the verbosity is properly set, then
 * it simply outputs the grammar that was read from its own point of view.
 *
 */

#include "ast.h"
#include "pointer_list.h"
#include "alloc.h"
#include "trace.h"
#include "errors.h"
#include "master_list.h"

static void _grammar(ast_grammar_t* node);
static void _grammar_item(ast_grammar_item_t* node);
static void _directive(ast_directive_t* node);
static void _rule(ast_rule_t* node);
static void _alternative(ast_alternative_t* node);
static void _alternative_list(ast_alternative_list_t* node);
static void _rule_element_list(ast_rule_element_list_t* node);
static void _rule_element(ast_rule_element_t* node);
static void _list_clause(ast_list_clause_t* node);


static void _grammar(ast_grammar_t* node) {

    int mark = 0;
    ast_node_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        _grammar_item((ast_grammar_item_t*)item);
}

static void _grammar_item(ast_grammar_item_t* node) {

    if(node->node->type == NTERM_DIRECTIVE)
        _directive((ast_directive_t*)node->node);
    else if(node->node->type == NTERM_RULE)
        _rule((ast_rule_t*)node->node);
    else
        FATAL("unknown node type in _grammar_item(): %d", node->node->type);
}

static void _directive(ast_directive_t* node) {

    PRINT("# line number %d\n", node->dir_type->line_no);
    PRINT("%s {%s}\n\n", node->dir_type->raw_str->buffer, node->code->raw_str->buffer);
}

static void _rule(ast_rule_t* node) {

    PRINT("# line number %d\n%s\n", node->nterm->line_no, node->nterm->raw_str->buffer);
    _alternative_list(node->alt_list);
}

static void _alternative(ast_alternative_t* node) {

    if(node->re_list != NULL)
        _rule_element_list(node->re_list);
    else
        PRINT("%%error ");

    // code is always required
    PRINT("{%s}\n", node->code->raw_str->buffer);
}

static void _alternative_list(ast_alternative_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    PRINT("    : ");
    while(NULL != (item = iterate_ptr_list(node->list, &mark))) {
        if(mark > 1)
            PRINT("    | ");
        _alternative((ast_alternative_t*)item);
    }
    PRINT("    ;\n\n");
}

static void _rule_element_list(ast_rule_element_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        _rule_element((ast_rule_element_t*)item);
}

static void _rule_element(ast_rule_element_t* node) {

    if(node->token != NULL) {
        switch(node->token->type) {
            case TERM_NON_TERMINAL:
            case TERM_PRETEXT:
            case TERM_POSTTEXT:
            case TERM_NTERM_DEF:
            case TERM_TERM_DEF:
            case TERM_PRECODE:
            case TERM_POSTCODE:
            case TERM_ERROR:
                PRINT("%s ", node->token->raw_str->buffer);
                break;
            default:
                PRINT("%s ", node->token->type_str->buffer);
                break;
        }
    }
    else if(node->clause != NULL)
        _list_clause(node->clause);
    else
        FATAL("invalid node in _rule_element()");
}

static void _list_clause(ast_list_clause_t* node) {

    PRINT("%%list (");
    _rule_element_list(node->re_list);
    PRINT(") ");
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

void traverse_ast(void) {

    LOCAL_VERBOSITY(0);
    TRACE_HEADER;

    _grammar(get_master_list_root_node());
}

