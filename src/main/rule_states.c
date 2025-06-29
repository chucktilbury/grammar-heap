
#include "errors.h"
#include "nterm_list.h"
#include "parser.h"

static nterm_item_t* crnt_item;

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

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);
}

static void rule_element(rule_element_t* node) {

    if(node->token != NULL) {
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
                FATAL("unknown node type: %d", node->nterm->type);
        }
    }
    else if(node->nterm != NULL) {
        switch(node->nterm->type) {
            case NTERM_OR_FUNCTION:
                or_function((or_function_t*)node->nterm);
                break;

            case NTERM_ZERO_OR_MORE_FUNCTION:
                zero_or_more_function((zero_or_more_function_t*)node->nterm);
                break;

            case NTERM_ZERO_OR_ONE_FUNCTION:
                zero_or_one_function((zero_or_one_function_t*)node->nterm);
                break;

            case NTERM_ONE_OR_MORE_FUNCTION:
                one_or_more_function((one_or_more_function_t*)node->nterm);
                break;

            case NTERM_GROUPING_FUNCTION:
                grouping_function((grouping_function_t*)node->nterm);
                break;

            case NTERM_INLINE_CODE:
                // do nothing
                break;

            default:
                FATAL("unknown node type: %d", node->nterm->type);
        }
    }
    else
        FATAL("invalid rule element");
}

static void or_function(or_function_t* node) {

    rule_element(node->left);
    rule_element(node->right);
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    rule_element(node->rule_element);
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    rule_element(node->rule_element);
}


static void one_or_more_function(one_or_more_function_t* node) {

    rule_element(node->rule_element);
}

static void grouping_function(grouping_function_t* node) {

    rule_element_list(node->rule_element_list);
}

void detect_recursion(void) {

    int mark = 0;
    nterm_item_t* nterm;
    while(NULL != (nterm = iterate_nterm_list(&mark))) {
        crnt_item = nterm;
        grouping_function((grouping_function_t*)nterm->node);
    }
}
