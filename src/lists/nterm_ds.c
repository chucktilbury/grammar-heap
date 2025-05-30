/**
 * @file nterm_ds.c
 *
 * @brief AST pass that collects the data elements for a rule that is
 * being processed.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */
#include "ast.h"
#include "parser.h"

#include "common.h"
#include "nterm_list.h"
#include "lists.h"

// Note that if it's in an OR, and it's in a group, and it has the same name,
// then it's a duplicate.

typedef struct _func_stack_t_ {
    ast_type_t type;
    struct _func_stack_t_* next;
} func_stack_t;

static func_stack_t* func_stack = NULL;
static int stack_depth = 0;
static void push_func_stack(ast_type_t type) {

    ENTER;
    TRACE("stack depth: %d", stack_depth);
    func_stack_t* ptr = _ALLOC_TYPE(func_stack_t);
    ptr->type = type;
    ptr->next = NULL;

    if(func_stack != NULL)
        ptr->next = func_stack;
    func_stack = ptr;
    stack_depth++;
    RETURN();
}

static int peek_func_stack(void) {

    ENTER;
    if(func_stack == NULL)
        RETURN(-1);
    else
        RETURN(func_stack->type);
}

static int pop_func_stack(void) {

    ENTER;
    func_stack_t* ptr = func_stack;
    if(ptr != NULL) {
        func_stack = ptr->next;
        _FREE(ptr);
    }

    stack_depth--;
    TRACE("stack depth: %d", stack_depth);
    int rtv = peek_func_stack();
    RETURN(rtv);
}

static nterm_item_t* crnt_rule = NULL;

// add if unique
static void add_list(ptr_list_t* lst, nterm_ds_type_t* ptr) {

    // nterm_ds_type_t* item;
    // int mark = 0;
    //
    // while(NULL != (item = iterate_ptr_list(lst, &mark))) {
    //     if(!comp_string(ptr->name, item->name))
    //         return; // do not add a match
    // }

    append_ptr_list(lst, ptr);
}

static void add_terminal(string_t* name) {

    ENTER;
    nterm_ds_type_t* ptr = _ALLOC_TYPE(nterm_ds_type_t);
    if(name == NULL)
        ptr->name = create_string("OPER");
    else
        ptr->name = name;
    ptr->type = 1;

    TRACE("token: %s", name->buffer);
    add_list(crnt_rule->ds_names, ptr);

    RETURN();
}

static void add_non_terminal(nterm_item_t* item) {

    ENTER;
    nterm_ds_type_t* ptr = _ALLOC_TYPE(nterm_ds_type_t);
    ptr->name = item->nterm;
    ptr->type = 0;

    TRACE("token: %s", item->nterm->buffer);
    add_list(crnt_rule->ds_names, ptr);
    RETURN();
}

static void rule_element_list(rule_element_list_t* node);
static void rule_element(rule_element_t* node);
static void or_function(or_function_t* node);
static void zero_or_more_function(zero_or_more_function_t* node);
static void zero_or_one_function(zero_or_one_function_t* node);
static void one_or_more_function(one_or_more_function_t* node);
static void grouping_function(grouping_function_t* node);

static void rule_element_list(rule_element_list_t* node) {

    ENTER;
    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);
    RETURN();
}

const char* tok_to_str(int);
static void rule_element(rule_element_t* node) {

    ENTER;
    if(node->token != NULL) {
        switch(node->token->type) {
            case TERMINAL_KEYWORD:
                break;
            case TERMINAL_SYMBOL:
                add_terminal(node->token->str);
                break;
            case TERMINAL_OPER:
                // add_terminal(NULL);
                add_terminal(create_string(tok_to_str(node->token->type)));
                break;
            case NON_TERMINAL: {
                nterm_item_t* item = find_nterm(master_list->nterm_list, node->token->str->buffer);
                if(item == NULL) {
                    fprintf(stderr, "syntax error: undefined non-terminal \"%s\" on line %d\n",
                            node->token->str->buffer, node->token->line_no);
                    exit(1);
                }
                add_non_terminal(item);
            } break;
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
    RETURN();
}

static void or_function(or_function_t* node) {

    ENTER;
    push_func_stack(AST_OR_FUNCTION);
    rule_element(node->left);
    rule_element(node->right);
    pop_func_stack();
    RETURN();
}

static void zero_or_more_function(zero_or_more_function_t* node) {

    ENTER;
    push_func_stack(AST_ZERO_OR_MORE_FUNCTION);
    rule_element(node->rule_element);
    pop_func_stack();
    RETURN();
}

static void zero_or_one_function(zero_or_one_function_t* node) {

    ENTER;
    push_func_stack(AST_ZERO_OR_ONE_FUNCTION);
    rule_element(node->rule_element);
    pop_func_stack();
    RETURN();
}


static void one_or_more_function(one_or_more_function_t* node) {

    ENTER;
    push_func_stack(AST_ONE_OR_MORE_FUNCTION);
    rule_element(node->rule_element);
    pop_func_stack();
    RETURN();
}

static void grouping_function(grouping_function_t* node) {

    ENTER;
    push_func_stack(AST_GROUPING_FUNCTION);
    rule_element_list(node->rule_element_list);
    pop_func_stack();
    RETURN();
}

void find_ds(void) {

    TRACE("\n\n------------\nbegin add ds\n-------------");
    ENTER;
    int mark = 0;
    nterm_item_t* item;

    push_func_stack(AST_GRAMMAR_RULE);
    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        crnt_rule = item;
        TRACE("rule name: %s", item->nterm->buffer);
        grouping_function((grouping_function_t*)item->node);
    }
    pop_func_stack();
    RETURN();
}
