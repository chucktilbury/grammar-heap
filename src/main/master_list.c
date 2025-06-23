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
#include "master_list.h"
#include "parser.h"
#include "alloc.h"

#include "nterm_list.h"
#include "string_buffer.h"
#include "term_list.h"
//#include "nterm_comment.h"

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
static void inline_code(inline_code_t* node);
static void directive(directive_t* node);

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

static void directive(directive_t* node) {

    ENTER;

    // type is an int and code is a token
    TRACE("type: %s", token_to_str(node->type));

    switch(node->type) {
        case PRETEXT:
            append_string_fmt(master_list->pre_text, "\n// %d pre-text directive seen.\n", node->code->line_no);
            append_string_str(master_list->pre_text, node->code->str);
            break;

        case PRECODE:
            append_string_fmt(master_list->pre_code, "\n// %d pre-code directive seen.\n", node->code->line_no);
            append_string_str(master_list->pre_code, node->code->str);
            break;

        case POSTCODE:
            append_string_fmt(master_list->post_code, "\n// %d post-code directive seen.\n", node->code->line_no);
            append_string_str(master_list->post_code, node->code->str);
            break;

        default:
            FATAL("invalid directive type: %d", node->type);
    }

    TRACE("code: \n%s\n", raw_string(node->code->str));

    RETURN();
}

/*
non_terminal_rule
 : NON_TERMINAL grouping_function
 ;
 */
static void grammar_rule(grammar_rule_t* node) {

    ENTER;

    if(node->NON_TERMINAL != NULL) {
        TRACE_TOKEN(node->NON_TERMINAL);

        string_t* type = create_string_fmt("AST_%s", node->NON_TERMINAL->str->buffer);
        upcase(type);

        nterm_item_t* item = create_nterm_item(node->NON_TERMINAL->str, type, (ast_node_t*)node->grouping_function);
        append_nterm_list(master_list->nterm_list, item);

        grouping_function(node->grouping_function);
    }
    else
        directive(node->directive);

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
            case TERMINAL_KEYWORD:
                append_term_list(master_list->term_list, create_term_item(node->token, 1));
                break;

            case TERMINAL_OPER:
                append_term_list(master_list->term_list, create_term_item(node->token, 1));
                break;

            case TERMINAL_SYMBOL:
                append_term_list(master_list->term_list, create_term_item(node->token, 0));
                break;

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

            case AST_INLINE_CODE:
                inline_code((inline_code_t*)node->nterm);
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

static void inline_code(inline_code_t* node) {

    ENTER;

    //grouping_function(node->group);
    TRACE_TOKEN(node->code);

    RETURN();
}


static void raw_list(void) {

    ENTER;

    grammar(root_node);

    RETURN();
}

/**
 * @brief Create all of the basic lists.
 *
 * @param node
 */
void make_raw_lists(void) {

    LOCAL_VERBOSITY(19);
    HEADER;
    ENTER;

    master_list = create_master_list();

    raw_list();

    master_list->first_nterm = master_list->nterm_list->buffer[0];
    sort_nterm_list(master_list->nterm_list);
    sort_term_list(master_list->term_list);

    RETURN();
}

master_list_t* create_master_list(void) {

    ENTER;
    master_list_t* ptr = _ALLOC_TYPE(master_list_t);

    ptr->first_nterm = NULL;
    ptr->nterm_list = create_nterm_list();
    ptr->term_list = create_term_list();
    ptr->pre_text = create_string(NULL);
    ptr->pre_code = create_string(NULL);
    ptr->post_code = create_string(NULL);

    RETURN(ptr);
}

void destroy_master_list(master_list_t* lst) {

    ENTER;
    if(lst != NULL) {
        destroy_nterm_list(lst->nterm_list);
        destroy_term_list(lst->term_list);
        destroy_string(lst->pre_text);
        destroy_string(lst->pre_code);
        destroy_string(lst->post_code);
        _FREE(lst);
    }
    RETURN();
}

void dump_master_list(void) {

    LOCAL_VERBOSITY(1);
    HEADER;

    PRINT("");
    SEPARATOR;
    PRINT("PRE-TEXT:");
    emit_string(get_trace_handle(), master_list->pre_text);

    PRINT("");
    SEPARATOR;
    PRINT("PRE-CODE:");
    emit_string(get_trace_handle(), master_list->pre_code);

    PRINT("");
    SEPARATOR;
    PRINT("POST-CODE:");
    emit_string(get_trace_handle(), master_list->post_code);

    PRINT("");
    SEPARATOR;
    PRINT("TERMINAL LIST:");

    int mark = 0;
    term_item_t* titem;
    while(NULL != (titem = iterate_term_list(master_list->term_list, &mark)))
        PRINT("\t%3d. %-20s%s", mark, titem->token->ptype->buffer, titem->token->str->buffer);


    PRINT("");
    SEPARATOR;
    PRINT("NON-TERMINAL NODE LIST:");

    PRINT("ROOT NODE:   %-20s%-25s%p",
          master_list->first_nterm->nterm->buffer,
          master_list->first_nterm->type->buffer,
          (void*)master_list->first_nterm->node);

    mark = 0;
    nterm_item_t* nitem;
    while(NULL != (nitem = iterate_nterm_list(master_list->nterm_list, &mark)))
        PRINT("\t%3d. %-20s%-25s%p", mark, nitem->nterm->buffer, nitem->type->buffer, (void*)nitem->node);

    PRINT("");
}
