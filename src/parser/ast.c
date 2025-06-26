/**
 * @file ast.c
 *
 * @brief Abstract Syntax Tree
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-01
 * @copyright Copyright (c) 2025
 */

#include "ast.h"
#include "parser.h"
#include "errors.h"
#include "master_list.h"

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
static void directive(directive_t* node);
static void inline_code(inline_code_t* node);

/*
 grammar
 : grammar_list
 ;
 */
static void grammar(grammar_t* node) {

    grammar_list(node->grammar_list);
}


/*
 grammar_list
 : non_terminal_rule  SEMI
 | grammar non_terminal_rule SEMI
 ;
 */
static void grammar_list(grammar_list_t* node) {

    int mark = 0;
    grammar_rule_t* rule;

    while(NULL != (rule = (grammar_rule_t*)iterate_ast_node_list(node->list, &mark)))
        grammar_rule(rule);

}


/*
grammar_rule
 : NON_TERMINAL grouping_function
 ;
 */
static void grammar_rule(grammar_rule_t* node) {


    if(node->NON_TERMINAL != NULL) {
        fprintf(stdout, "%s ", node->NON_TERMINAL->str->buffer);
        grouping_function(node->grouping_function);
        fprintf(stdout, "\n\n");
    }
    else
        directive(node->directive);

}

static void directive(directive_t* node) {


    // type is an int and code is a token
    fprintf(stdout, "%s {", token_to_str(node->type));
    fprintf(stdout, "%s}\n\n", raw_string(node->code->str));

}

/*
rule_element_list
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
rule_element
 : NON_TERMINAL
 | TERMINAL_NAME
 | TERMINAL_OPER
 | TERMINAL_SYMBOL
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
            case TERMINAL_OPER:
            case TERMINAL_SYMBOL:
                fprintf(stdout, " %s", node->token->ptype->buffer);
                break;
            case NON_TERMINAL:
                fprintf(stdout, " %s", node->token->str->buffer);
                break;
            default:
                FATAL("unknown terminal type: %s", tok_to_str(node->token->type));
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
                FATAL("unknown non-terminal type: %s", nterm_to_str(node->nterm->type));
        }
    }
    else
        FATAL("invalid rule element");

}

/*
or_function
 : rule_element PIPE rule_element
 ;
 */
static void or_function(or_function_t* node) {

    rule_element(node->left);
    fprintf(stdout, " |");
    rule_element(node->right);
}

/*
zero_or_more_function
 : rule_element QUESTION
 ;
 */
static void zero_or_more_function(zero_or_more_function_t* node) {

    rule_element(node->rule_element);
    fprintf(stdout, "*");
}

/*
zero_or_one_function
 : rule_element STAR
 ;
 */
static void zero_or_one_function(zero_or_one_function_t* node) {

    rule_element(node->rule_element);
    fprintf(stdout, "?");
}


/*
one_or_more_function
 : rule_element PLUS
 ;
 */
static void one_or_more_function(one_or_more_function_t* node) {

    rule_element(node->rule_element);
    fprintf(stdout, "+");
}

/*
grouping_function
 : OPAREN rule_element_list CPAREN
 ;
 */
static void grouping_function(grouping_function_t* node) {

    fprintf(stdout, "(");
    rule_element_list(node->rule_element_list);
    fprintf(stdout, " )");
}

static void inline_code(inline_code_t* node) {

    //grouping_function(node->group);
    fprintf(stdout, " CODE_BLOCK");
}

#include "alloc.h"

static size_t get_node_size(ast_type_t type) {

    return (type == NTERM_GRAMMAR)                ? sizeof(grammar_t) :
            (type == NTERM_GRAMMAR_LIST)          ? sizeof(grammar_list_t) :
            (type == NTERM_GRAMMAR_RULE)          ? sizeof(grammar_rule_t) :
            (type == NTERM_RULE_ELEMENT_LIST)     ? sizeof(rule_element_list_t) :
            (type == NTERM_RULE_ELEMENT)          ? sizeof(rule_element_t) :
            (type == NTERM_OR_FUNCTION)           ? sizeof(or_function_t) :
            (type == NTERM_ZERO_OR_MORE_FUNCTION) ? sizeof(zero_or_more_function_t) :
            (type == NTERM_ZERO_OR_ONE_FUNCTION)  ? sizeof(zero_or_one_function_t) :
            (type == NTERM_ONE_OR_MORE_FUNCTION)  ? sizeof(one_or_more_function_t) :
            (type == NTERM_GROUPING_FUNCTION)     ? sizeof(grouping_function_t) :
            (type == NTERM_DIRECTIVE)     ? sizeof(directive_t) :
            (type == NTERM_INLINE_CODE)     ? sizeof(inline_code_t) :
            (size_t)-1;
}

ast_node_t* create_ast_node(ast_type_t type) {

    ast_node_t* node = _ALLOC(get_node_size(type));
    node->type = type;

    return node;
}

void traverse_ast(void) {

    printf("------------------------------------------------------------------\n");
    printf("---\ttraverse the AST\n");
    printf("------------------------------------------------------------------\n");

    if(errors == 0)
        grammar((grammar_t*)get_master_list()->root_node);
}

ast_node_list_t* create_ast_node_list(void) {

    return (ast_node_list_t*)create_ptr_list();
}

void append_ast_node_list(ast_node_list_t* lst, ast_node_t* ptr) {

    append_ptr_list((pointer_list_t*)lst, (void*)ptr);
}

ast_node_t* iterate_ast_node_list(ast_node_list_t* lst, int* post) {

    return (ast_node_t*)iterate_ptr_list((pointer_list_t*)lst, post);
}

int len_ast_node_list(ast_node_list_t* lst) {

    return len_ptr_list((pointer_list_t*)lst);
}

const char* nterm_to_str(ast_type_t type) {

    return (type == NTERM_GRAMMAR)                ? "NTERM_GRAMMAR" :
            (type == NTERM_GRAMMAR_LIST)          ? "NTERM_GRAMMAR_LIST" :
            (type == NTERM_GRAMMAR_RULE)          ? "NTERM_GRAMMAR_RULE" :
            (type == NTERM_RULE_ELEMENT_LIST)     ? "NTERM_RULE_ELEMENT_LIST" :
            (type == NTERM_RULE_ELEMENT)          ? "NTERM_RULE_ELEMENT" :
            (type == NTERM_OR_FUNCTION)           ? "NTERM_OR_FUNCTION" :
            (type == NTERM_ZERO_OR_MORE_FUNCTION) ? "NTERM_ZERO_OR_MORE_FUNCTION" :
            (type == NTERM_ZERO_OR_ONE_FUNCTION)  ? "NTERM_ZERO_OR_ONE_FUNCTION" :
            (type == NTERM_ONE_OR_MORE_FUNCTION)  ? "NTERM_ONE_OR_MORE_FUNCTION" :
            (type == NTERM_GROUPING_FUNCTION)     ? "NTERM_GROUPING_FUNCTION" :
            (type == NTERM_DIRECTIVE)     ? "NTERM_DIRECTIVE" :
            (type == NTERM_INLINE_CODE)     ? "NTERM_INLINE_CODE" :
            "UNKNOWN";
}
