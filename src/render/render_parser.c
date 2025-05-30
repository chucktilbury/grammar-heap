/**
 * @file gurge.c
 *
 * @brief AST pass that prints the regurgitates the input after processing.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */
#include "common.h"
#include "ast.h"
#include "parser.h"
#include "lists.h"

// #include "common.h"
#include "ptr_list.h"
#include "render_init.h"
#include "render_parser.h"
#include "strg_list.h"
#include "strgs.h"
#include "templates.h"

static void render_rules(FILE* fp, string_list_t* lst) {

    int mark = 0;
    string_t* str;
    while(NULL != (str = iterate_string_list(lst, &mark)))
        emit_string(fp, str);
}

static void rule_element_list(FILE* fp, rule_element_list_t* node);
static void rule_element(FILE* fp, rule_element_t* node);
static void or_function(FILE* fp, or_function_t* node);
static void zero_or_more_function(FILE* fp, zero_or_more_function_t* node);
static void zero_or_one_function(FILE* fp, zero_or_one_function_t* node);
static void one_or_more_function(FILE* fp, one_or_more_function_t* node);
static void grouping_function(FILE* fp, grouping_function_t* node);

static void rule_element_list(FILE* fp, rule_element_list_t* node) {

    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(fp, rule);
}

static void rule_element(FILE* fp, rule_element_t* node) {

    if(node->token != NULL) {
        switch(node->token->type) {
            case TERMINAL_SYMBOL:
            case TERMINAL_OPER:
            case TERMINAL_KEYWORD: {
                // term_item_t* term = find_term(master_list->term_list, node->token->str->buffer);
                //  fprintf(fp, "// %*sterminal rule element: %s\n", level * indent, "", term->token->buffer);
            } break;
            case NON_TERMINAL:
                // fprintf(fp, "// %*snon-terminal rule element: %s\n", level * indent, "", node->token->str->buffer);
                break;
            default:
                FATAL("unknown terminal type: %s", tok_to_str(node->token->type));
        }
    }
    else if(node->nterm != NULL) {
        switch(node->nterm->type) {
            case AST_OR_FUNCTION:
                or_function(fp, (or_function_t*)node->nterm);
                break;
            case AST_ZERO_OR_MORE_FUNCTION:
                zero_or_more_function(fp, (zero_or_more_function_t*)node->nterm);
                break;
            case AST_ZERO_OR_ONE_FUNCTION:
                zero_or_one_function(fp, (zero_or_one_function_t*)node->nterm);
                break;
            case AST_ONE_OR_MORE_FUNCTION:
                one_or_more_function(fp, (one_or_more_function_t*)node->nterm);
                break;
            case AST_GROUPING_FUNCTION:
                grouping_function(fp, (grouping_function_t*)node->nterm);
                break;
            default:
                FATAL("unknown non-terminal type: %s", nterm_to_str(node->nterm->type));
        }
    }
    else
        FATAL("invalid rule element");
}

static void or_function(FILE* fp, or_function_t* node) {

    rule_element(fp, node->left);
    rule_element(fp, node->right);
    render_rules(fp, node->impl);
}

static void zero_or_more_function(FILE* fp, zero_or_more_function_t* node) {

    rule_element(fp, node->rule_element);
    render_rules(fp, node->impl);
}

static void zero_or_one_function(FILE* fp, zero_or_one_function_t* node) {

    rule_element(fp, node->rule_element);
    render_rules(fp, node->impl);
}


static void one_or_more_function(FILE* fp, one_or_more_function_t* node) {

    rule_element(fp, node->rule_element);
    render_rules(fp, node->impl);
}

static void grouping_function(FILE* fp, grouping_function_t* node) {

    rule_element_list(fp, node->rule_element_list);
    render_rules(fp, node->impl);
}

void emit_parser_implementation(FILE* fp) {

    // grouping_function((grouping_function_t*)((nterm_item_t*)(master_list->nterm_list->buffer[master_list->nterm_idx]))->node);
    grouping_function(fp, (grouping_function_t*)master_list->crnt_nterm->node);
}
