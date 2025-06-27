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

static master_list_t* master_list = NULL;

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
 * grammar
 * : grammar_list
 * ;
 */
static void grammar(grammar_t* node) {

    grammar_list(node->grammar_list);
}


/*
 * grammar_list
 * : non_terminal_rule  SEMI
 * | grammar non_terminal_rule SEMI
 * ;
 */
static void grammar_list(grammar_list_t* node) {

    int mark = 0;
    grammar_rule_t* rule;

    while(NULL != (rule = (grammar_rule_t*)iterate_ast_node_list(node->list, &mark)))
        grammar_rule(rule);
}

static void directive(directive_t* node) {

    // type is an int and code is a token
    switch(node->type) {
        case PRETEXT:
            append_string_fmt(master_list->pre_text, "\n// %d pre-text directive seen.\n", node->code->line_no);
            append_string_str(master_list->pre_text, node->code->str);
            break;
        case POSTTEXT:
            append_string_fmt(master_list->pre_text, "\n// %d post-text directive seen.\n", node->code->line_no);
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
        case REQUIRES:
            append_string_fmt(master_list->post_code, "\n// %d requires directive seen.\n", node->code->line_no);
            append_string_str(master_list->post_code, node->code->str);
            break;
        case PROVIDES:
            append_string_fmt(master_list->post_code, "\n// %d provides directive seen.\n", node->code->line_no);
            append_string_str(master_list->post_code, node->code->str);
            break;
        case TERM_DEF:
            append_string_fmt(master_list->post_code, "\n// %d terminal directive seen.\n", node->code->line_no);
            append_string_str(master_list->post_code, node->code->str);
            break;
        case NTERM_DEF:
            append_string_fmt(master_list->post_code, "\n// %d non-terminal directive seen.\n", node->code->line_no);
            append_string_str(master_list->post_code, node->code->str);
            break;

        default:
            FATAL("invalid directive type: %d", node->type);
    }
}

/*
 n *on_terminal_rule
 : NON_TERMINAL grouping_function
 ;
 */
static void grammar_rule(grammar_rule_t* node) {

    if(node->NON_TERMINAL != NULL) {
        string_t* type = create_string_fmt("NTERM_%s", node->NON_TERMINAL->str->buffer);
        upcase(type);

        nterm_item_t* item = create_nterm_item(node->NON_TERMINAL->str, type, (ast_node_t*)node->grouping_function);
        append_nterm_list(item);

        grouping_function(node->grouping_function);
    }
    else
        directive(node->directive);
}


/*
 r *ule_element_list
 : rule_element
 | rule_element_list rule_element
 ;
 */
static void rule_element_list(rule_element_list_t* node) {

    int mark = 0;
    rule_element_t* rule;

    while(NULL != (rule = (rule_element_t*)iterate_ast_node_list(node->list, &mark)))
        rule_element(rule);
}


/*
 r *ule_element
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

    if(node->token != NULL) {

        switch(node->token->type) {
            case TERMINAL_KEYWORD:
                append_term_list(create_term_item(node->token, 1));
                break;

            case TERMINAL_OPER:
                append_term_list(create_term_item(node->token, 1));
                break;

            case TERMINAL_SYMBOL:
                append_term_list(create_term_item(node->token, 0));
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
                inline_code((inline_code_t*)node->nterm);
                break;

            default:
                FATAL("unknown node type: %d", node->nterm->type);
        }
    }
    else
        FATAL("invalid rule element");
}

/*
 o *r_function
 : rule_element PIPE rule_element
 ;
 */
static void or_function(or_function_t* node) {

    rule_element(node->left);
    rule_element(node->right);
}

/*
 z *ero_or_more_function
 : rule_element QUESTION
 ;
 */
static void zero_or_more_function(zero_or_more_function_t* node) {

    rule_element(node->rule_element);
}

/*
 z *ero_or_one_function
 : rule_element STAR
 ;
 */
static void zero_or_one_function(zero_or_one_function_t* node) {

    rule_element(node->rule_element);
}


/*
 o *ne_or_more_function
 : rule_element PLUS
 ;
 */
static void one_or_more_function(one_or_more_function_t* node) {

    rule_element(node->rule_element);
}

/*
 g *rouping_function
 : OPAREN rule_element_list CPAREN
 ;
 */
static void grouping_function(grouping_function_t* node) {

    rule_element_list(node->rule_element_list);
}

static void inline_code(inline_code_t* node) {

    (void)node;
    //grouping_function(node->group);
}


static void raw_list(void) {

    grammar((grammar_t*)master_list->root_node);
}

/**
 * @brief Create all of the basic lists.
 *
 * @param node
 */
void make_raw_lists(void) {

    raw_list();

    master_list->first_nterm = master_list->nterm_list->buffer[0];
    sort_nterm_list();
    sort_term_list();
}

void init_master_list(void) {

    master_list = _ALLOC_TYPE(master_list_t);

    master_list->first_nterm = NULL;
    master_list->nterm_list = create_nterm_list();
    master_list->term_list = create_term_list();
    master_list->pre_text = create_string(NULL);
    master_list->post_text = create_string(NULL);
    master_list->pre_code = create_string(NULL);
    master_list->post_code = create_string(NULL);
    master_list->requires = create_string(NULL);
    master_list->provides = create_string(NULL);
    master_list->term_def = create_string(NULL);
    master_list->nterm_def = create_string(NULL);
}

master_list_t* get_master_list(void) {

    return master_list;
}

void set_root_node(void* ptr)  {

    master_list->root_node = ptr;
}

void* get_root_node(void) {

    return master_list->root_node;
}

void destroy_master_list(master_list_t* lst) {

    if(lst != NULL) {
        destroy_nterm_list();
        destroy_term_list();
        destroy_string(lst->pre_text);
        destroy_string(lst->post_text);
        destroy_string(lst->pre_code);
        destroy_string(lst->post_code);
        destroy_string(lst->requires);
        destroy_string(lst->provides);
        destroy_string(lst->term_def);
        destroy_string(lst->nterm_def);
        _FREE(lst);
    }
}

#include "trace.h"

void dump_master_list(void) {

    printf("------------------------------------------------------------------\n");
    printf("---\tDump the master list\n");
    printf("------------------------------------------------------------------\n");

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("PRE-TEXT:\n");
    emit_string(get_trace_handle(), master_list->pre_text);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("POST-TEXT:\n");
    emit_string(get_trace_handle(), master_list->post_text);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("PRE-CODE:\n");
    emit_string(get_trace_handle(), master_list->pre_code);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("POST-CODE:\n");
    emit_string(get_trace_handle(), master_list->post_code);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("PROVIDES:\n");
    emit_string(get_trace_handle(), master_list->provides);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("REQUIRES:\n");
    emit_string(get_trace_handle(), master_list->requires);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("TERM_DEF:\n");
    emit_string(get_trace_handle(), master_list->term_def);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("NTERM_DEF:\n");
    emit_string(get_trace_handle(), master_list->nterm_def);

    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("TERMINAL LIST:\n");

    int mark = 0;
    term_item_t* titem;
    while(NULL != (titem = iterate_term_list(&mark)))
        printf("\t%3d. %-20s%s\n", mark, titem->tok->ptype->buffer, titem->tok->str->buffer);


    printf("\n");
    printf("------------------------------------------------------------------\n");
    printf("NON-TERMINAL NODE LIST:\n");

    printf("ROOT NODE:   %-20s%-25s%p\n\n",
           master_list->first_nterm->nterm->buffer,
           master_list->first_nterm->type->buffer,
           (void*)master_list->first_nterm->node);

    mark = 0;
    nterm_item_t* nitem;
    while(NULL != (nitem = iterate_nterm_list(&mark)))
        printf("\t%3d. %-20s%-25s%p\n", mark, nitem->nterm->buffer, nitem->type->buffer, (void*)nitem->node);

    printf("\n");
}
