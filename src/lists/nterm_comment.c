/**
 * @file nterm_comment.c
 *
 * This traverses the AST to generate the comment that goes before the state
 * loop in a parser function. It is intended to be used to verify the validity
 * of the actual function implementation.
 */
#include "common.h"
#include "ast.h"
#include "parser.h"

#include "lists.h"
#include "strg_list.h"

#define LST crnt_rule->rule_comment
#define EMIT_LINE(...) append_string_list(LST, create_string_fmt(__VA_ARGS__))

static int level = 0;
static const int indent = 2;
static nterm_item_t* crnt_rule;

static void rule_element_list(rule_element_list_t* node);
static void rule_element(rule_element_t* node);
static void or_function(or_function_t* node);
static void zero_or_more_function(zero_or_more_function_t* node);
static void zero_or_one_function(zero_or_one_function_t* node);
static void one_or_more_function(one_or_more_function_t* node);
static void grouping_function(grouping_function_t* node);

static void rule_element_list(rule_element_list_t* node) {

    int mark = 0;
    rule_element_t* rule;
    // EMIT_LINE("// begin rule_element_list\n");

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);

    // EMIT_LINE("// end rule_element_list\n");
}

static void rule_element(rule_element_t* node) {

    // EMIT_LINE("// begin rule_element\n");

    if(node->token != NULL) {
        switch(node->token->type) {
            case TERMINAL_SYMBOL:
            case TERMINAL_OPER:
            case TERMINAL_KEYWORD: {
                term_item_t* term = find_term(master_list->term_list, node->token->str->buffer);
                EMIT_LINE("// %*sterminal rule element: %s\n", level * indent, "", term->token->buffer);
            } break;
            case NON_TERMINAL:
                EMIT_LINE("// %*snon-terminal rule element: %s\n", level * indent, "", node->token->str->buffer);
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

    // EMIT_LINE("// end rule_element\n");
}

static void or_function(or_function_t* node) {

    EMIT_LINE("// %*sbegin or_function (%d)\n", level * indent, "", node->state);
    level++;
    rule_element(node->left);
    rule_element(node->right);
    level--;
    EMIT_LINE("// %*send or_function (%d)\n", level * indent, "", node->state);
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    EMIT_LINE("// %*sbegin zero_or_more_function (%d)\n", level * indent, "", node->state);
    level++;
    rule_element(node->rule_element);
    level--;
    EMIT_LINE("// %*send zero_or_more_function (%d)\n", level * indent, "", node->state);
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    EMIT_LINE("// %*sbegin zero_or_one_function (%d)\n", level * indent, "", node->state);
    level++;
    rule_element(node->rule_element);
    level--;
    EMIT_LINE("// %*send zero_or_one_function (%d)\n", level * indent, "", node->state);
}


static void one_or_more_function(one_or_more_function_t* node) {

    EMIT_LINE("// %*sbegin one_or_more_function (%d)\n", level * indent, "", node->state);
    level++;
    rule_element(node->rule_element);
    level--;
    EMIT_LINE("// %*send one_or_more_function (%d)\n", level * indent, "", node->state);
}

static void grouping_function(grouping_function_t* node) {

    EMIT_LINE("// %*sbegin grouping_function (%d)\n", level * indent, "", node->state);
    level++;
    rule_element_list(node->rule_element_list);
    level--;
    EMIT_LINE("// %*send grouping_function (%d)\n", level * indent, "", node->state);
}

void add_comments(void) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        crnt_rule = item;
        grouping_function((grouping_function_t*)item->node);
    }
}
