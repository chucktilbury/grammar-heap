/**
 * @file generate.c
 *
 * @brief Render the interal state of a parser function using templates.
 *
 */
#include "ast.h"
#include "parser.h"
#include "lists.h"

#include "errors.h"
#include "nterm_list.h"
#include "string_list.h"

#define EMIT_LINE(fp, ...) append_string_list(fp, create_string_fmt(__VA_ARGS__))

static void rule_element_list(string_list_t* lst, rule_element_list_t* node);
static void rule_element(string_list_t* lst, rule_element_t* node);
static void or_function(or_function_t* node);
static void zero_or_more_function(zero_or_more_function_t* node);
static void zero_or_one_function(zero_or_one_function_t* node);
static void one_or_more_function(one_or_more_function_t* node);
static void grouping_function(grouping_function_t* node);

static void rule_element_list(string_list_t* lst, rule_element_list_t* node) {

    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(lst, rule);
}

static void rule_element(string_list_t* lst, rule_element_t* node) {

    if(node->token != NULL) {
        switch(node->token->type) {
            case TERMINAL_SYMBOL:
            case TERMINAL_OPER:
            case TERMINAL_KEYWORD: {
                term_item_t* term = find_term(master_list->term_list, node->token->str->buffer);
                EMIT_LINE(lst, "// terminal rule element: %s\n", term->token->buffer);
            } break;
            case NON_TERMINAL:
                EMIT_LINE(lst, "// non-terminal rule element: %s\n", node->token->str->buffer);
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
}

static void or_function(or_function_t* node) {

    EMIT_LINE(node->impl, "case %d:\n", node->state);
    rule_element(node->impl, node->left);
    rule_element(node->impl, node->right);
    EMIT_LINE(node->impl, "break;\n// end or_function rule at state %d\n\n", node->state);
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    EMIT_LINE(node->impl, "case %d:\n", node->state);
    rule_element(node->impl, node->rule_element);
    EMIT_LINE(node->impl, "break;\n// end zero_or_more_function rule at state %d\n\n", node->state);
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    EMIT_LINE(node->impl, "case %d:\n", node->state);
    rule_element(node->impl, node->rule_element);
    EMIT_LINE(node->impl, "break;\n// end zero_or_one_function rule at state %d\n\n", node->state);
}


static void one_or_more_function(one_or_more_function_t* node) {

    EMIT_LINE(node->impl, "case %d:\n", node->state);
    rule_element(node->impl, node->rule_element);
    EMIT_LINE(node->impl, "break;\n// end one_or_more_function rule at state %d\n\n", node->state);
}

static void grouping_function(grouping_function_t* node) {

    EMIT_LINE(node->impl, "case %d:\n", node->state);
    rule_element_list(node->impl, node->rule_element_list);
    EMIT_LINE(node->impl, "break;\n// end grouping_function rule at state %d\n\n", node->state);
}


void generate_rules(void) {

    int i = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_ptr_list(master_list->nterm_list, &i)))
        grouping_function((grouping_function_t*)item->node);
}
