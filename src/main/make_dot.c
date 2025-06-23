/*
 * Create the dot file for each non-terminal node.
 */
#include <stdio.h>

#include "make_dot.h"
#include "errors.h"
#include "nterms.h"
#include "pointer_list.h"
#include "tokens.h"

static void _alternative(ast_alternative_t* node);
static void _alternative_list(ast_alternative_list_t* node);
static void _rule_element_list(ast_rule_element_list_t* node);
static void _rule_element(ast_rule_element_t* node);
static void _list_clause(ast_list_clause_t* node);

static FILE* outfp;
static int first;

static void _alternative(ast_alternative_t* node) {

    if(node->re_list != NULL)
        _rule_element_list(node->re_list);

    if(node->code != NULL)
        fprintf(outfp, " -> CODE_BLOCK");
}

static void _alternative_list(ast_alternative_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    first = 0;

    while(NULL != (item = iterate_ptr_list(node->list, &mark))) {
        fprintf(outfp, "START -> ");
        _alternative((ast_alternative_t*)item);
        fprintf(outfp, "\n");
    }
}

static void _rule_element_list(ast_rule_element_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark))) {
        _rule_element((ast_rule_element_t*)item);
        if(mark < len_ptr_list(node->list))
            fprintf(outfp, " -> ");
    }
}

static void _rule_element(ast_rule_element_t* node) {

    static int sn = 0;

    if(node->token != NULL) {
        if(node->token->type != TERM_NON_TERMINAL)
            fprintf(outfp, "%s%d ", node->token->type_str->buffer, sn++); //, node->token->type_str->buffer);
        else
            fprintf(outfp, "%s%d ", node->token->raw_str->buffer, sn++); //, node->token->raw_str->buffer);
    }
    else if(node->clause != NULL)
        _list_clause(node->clause);
    else
        FATAL("invalid node in _rule_element()");
}

static void _list_clause(ast_list_clause_t* node) {

    _rule_element_list(node->re_list);
}

string_t* create_dot(string_t* name) {

    outfp = stdout;
    string_t* str = create_string(NULL);

    outfp = fopen(name->buffer, "w");

    fprintf(outfp, "digraph {\nlabel=\"production name: %s\";\nrankdir=LR;\nlabelloc=t;\n\n", name->buffer);

    nterm_elem_t* nterm = find_nterm(name);
    _alternative_list((ast_alternative_list_t*)nterm->alts);

    fprintf(outfp, "CODE_BLOCK -> END\n}\n\n");

    fclose(outfp);

    return str;
}
