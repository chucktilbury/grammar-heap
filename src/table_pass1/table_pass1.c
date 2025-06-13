
#include "ast.h"
#include "master_list.h"
#include "nterm_list.h"
#include "parser.h"
#include "errors.h"
#include "trace.h"

#ifdef USE_TRACE
#define TRACE_TOKEN(t) TRACE("token: \"%s\": %s", raw_string((t)->str), tok_to_str((t)->type))
#else
#define TRACE_TOKEN(t)
#endif

static void rule_element_list(rule_element_list_t* node);
static void rule_element(rule_element_t* node);
static void or_function(or_function_t* node);
static void zero_or_more_function(zero_or_more_function_t* node);
static void zero_or_one_function(zero_or_one_function_t* node);
static void one_or_more_function(one_or_more_function_t* node);
static void grouping_function(grouping_function_t* node);
static void inline_code(inline_code_t* node);

static void rule_element_list(rule_element_list_t* node) {

    ENTER;
    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);

    RETURN();
}

static void rule_element(rule_element_t* node) {

    ENTER;

    if(node->token != NULL) {
        TRACE_TOKEN(node->token);
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
            case AST_INLINE_CODE:
                inline_code((inline_code_t*)node->nterm);
                break;

            default:
                FATAL("unknown non-terminal type: %s", nterm_to_str(node->nterm->type));
        }
    }
    else
        FATAL("invalid rule element");

    RETURN();
}

static void or_function(or_function_t* node) {

    ENTER;

    rule_element(node->left);
    rule_element(node->right);

    RETURN();
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}

static void one_or_more_function(one_or_more_function_t* node) {

    ENTER;

    rule_element(node->rule_element);

    RETURN();
}

static void grouping_function(grouping_function_t* node) {

    ENTER;

    rule_element_list(node->rule_element_list);

    RETURN();
}

static void inline_code(inline_code_t* node) {

    ENTER;

    //grouping_function(node->group);
    TRACE_TOKEN(node->code);

    RETURN();
}

void create_table_pass1(void) {

    LOCAL_VERBOSITY(9);
    HEADER;
    ENTER;

    int mark = 0;
    nterm_item_t* node;

    while(NULL != (node = iterate_nterm_list(master_list->nterm_list, &mark))) {
        PRINT("");
        HEADER;
        TRACE("node name: %s", node->nterm->buffer);
        grouping_function((grouping_function_t*)node->node);
    }

    PRINT("");
    RETURN();
}
