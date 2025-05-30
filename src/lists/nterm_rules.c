/**
 * @file nterm_func.c
 *
 * This traverses the AST to create the guts of a parser function.
 *
 */
#include "common.h"
#include "ast.h"
#include "parser.h"

#include "ptr_list.h"
#include "strg_list.h"
#include "strgs.h"
#include "lists.h"
#include "nterm_rules.h"

#define LST node->impl
#define EMIT_LINE(...) append_string_list(LST, create_string_fmt(__VA_ARGS__))

static int state;
static int level;

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

    if(node->nterm != NULL) {
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
}

static void or_function(or_function_t* node) {

    level++;

    node->state = state += 100;
    node->level = level;
    node->impl = create_string_list();

    EMIT_LINE("// begin or_function rule at state %d:%d\n", node->state, node->level);
    rule_element(node->left);
    rule_element(node->right);

    level--;
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    level++;

    node->state = state += 100;
    node->level = level;
    node->impl = create_string_list();

    EMIT_LINE("// begin zero_or_more_function rule at state %d:%d\n", node->state, node->level);
    rule_element(node->rule_element);

    level--;
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    level++;

    node->state = state += 100;
    node->level = level;
    node->impl = create_string_list();

    EMIT_LINE("// begin zero_or_one_function rule at state %d:%d\n", node->state, node->level);
    rule_element(node->rule_element);

    level--;
}


static void one_or_more_function(one_or_more_function_t* node) {

    level++;

    node->state = state += 100;
    node->level = level;
    node->impl = create_string_list();

    EMIT_LINE("// begin one_or_more_function rule at state %d:%d\n", node->state, node->level);
    rule_element(node->rule_element);

    level--;
}

static void grouping_function(grouping_function_t* node) {

    level++;
    node->state = state += 100;
    node->level = level;
    node->impl = create_string_list();

    EMIT_LINE("// begin grouping_function rule at state %d:%d\n", node->state, node->level);
    rule_element_list(node->rule_element_list);

    level--;
}

void init_rule_states(void) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        state = 900;
        level = -1;
        grouping_function((grouping_function_t*)item->node);
    }
}
