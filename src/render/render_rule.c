/**
 * @file render_rule.c
 *
 * @brief AST pass that prints the regurgitates the input after processing.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */

#include "ast.h"
#include "parser.h"
#include "lists.h"

#include "common.h"

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
                term_item_t* term = find_term(master_list->term_list, node->token->str->buffer);
                fprintf(fp, "%s ", term->token->buffer);
            } break;
            case NON_TERMINAL:
                fprintf(fp, "%s ", node->token->str->buffer);
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
    fprintf(fp, "| ");
    rule_element(fp, node->right);
}

static void zero_or_more_function(FILE* fp, zero_or_more_function_t* node) {

    rule_element(fp, node->rule_element);
    fprintf(fp, "* ");
}

static void zero_or_one_function(FILE* fp, zero_or_one_function_t* node) {

    rule_element(fp, node->rule_element);
    fprintf(fp, "? ");
}


static void one_or_more_function(FILE* fp, one_or_more_function_t* node) {

    rule_element(fp, node->rule_element);
    fprintf(fp, "+ ");
}

static void grouping_function(FILE* fp, grouping_function_t* node) {

    fprintf(fp, "( ");
    rule_element_list(fp, node->rule_element_list);
    fprintf(fp, ") ");
}

void dump_rule(FILE* fp, nterm_item_t* rule) {

    fprintf(fp, "%s ", rule->nterm->buffer);
    grouping_function(fp, (grouping_function_t*)rule->node);
}
