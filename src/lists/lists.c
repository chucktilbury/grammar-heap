/**
 * @file lists.c
 *
 * @brief Traverse the AST to create the base lists. Terminals and
 * nonterminals as well as their associated objects.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */

#include "ast.h"
#include "errors.h"
#include "lists.h"
#include "parser.h"
#include "alloc.h"

#include "nterm_list.h"
#include "nterm_ds.h"
#include "term_list.h"
#include "nterm_comment.h"
#include "nterm_rules.h"

// global product produced by this file
master_list_t* master_list;

// #define USE_TRACE
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
non_terminal_rule
 : NON_TERMINAL grouping_function
 ;
 */
static void grammar_rule(grammar_rule_t* node) {

    ENTER;

    TRACE_TOKEN(node->NON_TERMINAL);

    string_t* type = create_string_fmt("AST_%s", node->NON_TERMINAL->str->buffer);
    upcase(type);
    append_nterm_list(master_list->nterm_list, create_nterm_item(node->NON_TERMINAL->str, type, (ast_node_t*)node->grouping_function));
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
 | non_terminal_rule_element
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
            case TERMINAL_KEYWORD: {
                string_t* term = copy_string(node->token->str);
                // strip_quotes(term);

                string_t* tok = copy_string(term);
                upcase(tok);
                // create_string copies the string
                // the string term is simply assigned.
                append_term_list(master_list->term_list, create_term_item(term, create_string_fmt("TOK_%s", tok->buffer), 1));
                destroy_string(tok);
            } break;
            case TERMINAL_OPER: {
                string_t* term = copy_string(node->token->str);
                // strip_quotes(term);

                string_t* tok = copy_string(term);
                tok = convert(tok);

                append_term_list(master_list->term_list, create_term_item(term, create_string_fmt("TOK_%s", tok->buffer), 1));

                destroy_string(tok); // normally GC would handle this.
            } break;
            case TERMINAL_SYMBOL: {
                string_t* term = copy_string(node->token->str);
                string_t* tok = create_string_fmt("TOK_%s", node->token->str->buffer);

                append_term_list(master_list->term_list, create_term_item(term, tok, 0));
            } break;
            case NON_TERMINAL:
                /* do nothing */
                break;
            default:
                FATAL("invalid token type: %s", tok_to_str(node->token->type));
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
                FATAL("unknown node type: %d", node->nterm->type);
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

static void raw_list(void) {

    grammar(root_node);
}

/**
 * @brief Create all of the basic lists.
 *
 * @param node
 */
void make_raw_lists(void) {

    master_list = create_master_list();

    raw_list();

    master_list->first_nterm = master_list->nterm_list->buffer[0];
    sort_nterm_list(master_list->nterm_list);
    sort_term_list(master_list->term_list);

    init_rule_states();
    add_comments();
    find_ds();
}

master_list_t* create_master_list(void) {

    master_list_t* ptr = _ALLOC_TYPE(master_list_t);

    ptr->first_nterm = NULL;
    ptr->current_file = NULL;
    ptr->nterm_list = create_nterm_list();
    ptr->term_list = create_term_list();

    return ptr;
}

void destroy_master_list(master_list_t* lst) {

    if(lst != NULL) {
        destroy_nterm_item(lst->first_nterm);
        destroy_nterm_list(lst->nterm_list);
        destroy_term_list(lst->term_list);
        destroy_string(lst->current_file);
        _FREE(lst);
    }
}
