/*
 * Master List
 *
 */
#include "ast.h"
#include "alloc.h"
#include "master_list.h"
//#include "parser.h"
#include "errors.h"
#include "pointer_list.h"
#include "string_buffer.h"
#include "tokens.h"
#include "trace.h"
#include "terms.h"
#include "nterms.h"
#include "make_dot.h"

static master_list_t* master_list = NULL;
static int first_rule_seen = 0;

// defined in parse.y
extern int errors;

static void _grammar(ast_grammar_t* node);
static void _grammar_item(ast_grammar_item_t* node);
static void _directive(ast_directive_t* node);
static void _rule(ast_rule_t* node);
static void _alternative(ast_alternative_t* node);
static void _alternative_list(ast_alternative_list_t* node);
static void _rule_element_list(ast_rule_element_list_t* node);
static void _rule_element(ast_rule_element_t* node);
static void _list_clause(ast_list_clause_t* node);


static void _grammar(ast_grammar_t* node) {

    int mark = 0;
    ast_node_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        _grammar_item((ast_grammar_item_t*)item);
}

static void _grammar_item(ast_grammar_item_t* node) {

    if(node->node->type == NTERM_DIRECTIVE)
        _directive((ast_directive_t*)node->node);
    else if(node->node->type == NTERM_RULE)
        _rule((ast_rule_t*)node->node);
    else
        FATAL("unknown node type in _grammar_item(): %d", node->node->type);
}

static void _directive(ast_directive_t* node) {

    switch(node->dir_type->type) {
        case TERM_POSTCODE:
            master_list->postcode = node->code->raw_str;
            break;
        case TERM_PRECODE:
            master_list->precode = node->code->raw_str;
            break;
        case TERM_PRETEXT:
            master_list->pretext = node->code->raw_str;
            break;
        case TERM_POSTTEXT:
            master_list->posttext = node->code->raw_str;
            break;
        case TERM_HEADER:
            master_list->headertext = node->code->raw_str;
            break;
        case TERM_TERM_DEF:
            master_list->term_def = node->code->raw_str;
            break;
        case TERM_NTERM_DEF:
            master_list->nterm_def = node->code->raw_str;
            break;
        default:
            FATAL("unknown node type: %d", node->dir_type->type);
    }
}

static void _rule(ast_rule_t* node) {

    if(first_rule_seen == 0) {
        master_list->start_node = node->nterm->raw_str;
        first_rule_seen++;
    }
    add_nterm_list(node->nterm->raw_str, node->alt_list);
    _alternative_list(node->alt_list);
}

static void _alternative(ast_alternative_t* node) {

    if(node->re_list != NULL)
        _rule_element_list(node->re_list);
}

static void _alternative_list(ast_alternative_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark))) {
        _alternative((ast_alternative_t*)item);
    }
}

static void _rule_element_list(ast_rule_element_list_t* node) {

    int mark = 0;
    ast_alternative_t* item;

    while(NULL != (item = iterate_ptr_list(node->list, &mark)))
        _rule_element((ast_rule_element_t*)item);
}

static void _rule_element(ast_rule_element_t* node) {

    if(node->token != NULL) {
        switch(node->token->type) {
            case TERM_NON_TERMINAL:
            case TERM_PRETEXT:
            case TERM_PRECODE:
            case TERM_POSTCODE:
            case TERM_ERROR:
                break;
            default: {
                // add a unique token
                int mark = 0;
                term_elem_t* ptr;
                while(NULL != (ptr = iterate_term_list(&mark)))
                    if(!comp_string(ptr->type_str, node->token->type_str))
                        return;
                add_term_list(node->token->raw_str, node->token->type_str, node->token->type);
            } break;
        }
    }
    else if(node->clause != NULL)
        _list_clause(node->clause);
    else
        FATAL("invalid node in _rule_element()");
}

static void _list_clause(ast_list_clause_t* node) {

    _rule_element_list(node->re_list);
}


/*
 * PUBLIC INTERFACE
 */

void set_master_list_root_node(void* node) {

    ASSERT(master_list != NULL, "master list is not initialized");
    master_list->root_node = node; // implicit cast
}

void* get_master_list_root_node(void) {

    ASSERT(master_list != NULL, "master list is not initialized");
    return master_list->root_node; // implicit cast
}

void init_master_list(void) {

    master_list = _ALLOC_TYPE(master_list_t);
    master_list->nterms = create_nterm_list();
    master_list->terms = create_term_list();
}

void create_master_list(void) {

    _grammar(master_list->root_node);

    // prepare them for the binary search functions.
    sort_term_list();
    sort_nterm_list();

    int mark = 0;
    nterm_elem_t* nterm;

    while(NULL != (nterm = iterate_nterm_list((&mark))))
        create_dot(nterm->name);
}

void destroy_master_list(void) {

    if(master_list != NULL) {
        int mark = 0;
        nterm_elem_t* nterm;
        while(NULL != (nterm = iterate_nterm_list(&mark)))
            destroy_nterm_elem(nterm);

        mark = 0;
        term_elem_t* term;
        while(NULL != (term = iterate_term_list(&mark)))
            destroy_term_elem(term);

        if(master_list->headertext != NULL)
            destroy_string(master_list->headertext);

        if(master_list->postcode != NULL)
            destroy_string(master_list->postcode);

        if(master_list->precode != NULL)
            destroy_string(master_list->precode);

        if(master_list->pretext != NULL)
            destroy_string(master_list->pretext);

        _FREE(master_list);
    }
}

master_list_t* get_master_list(void) {

    return master_list;
}

void dump_master_list(void) {

    int mark = 0;
    nterm_elem_t* nelem;
    term_elem_t* elem;

    SEPARATOR;
    PRINT("\tstarting non-terminal\n");
    SEPARATOR;
    PRINT("\t%-15s\n\n", master_list->start_node->buffer);

    SEPARATOR;
    PRINT("\tnon-terminals\n");
    SEPARATOR;
    while(NULL != (nelem = iterate_nterm_list(&mark)))
        PRINT("\t%3d. %-15s %p\n", mark, nelem->name->buffer, (void*)nelem->alts);
    PRINT("\n");

    mark = 0;
    SEPARATOR;
    PRINT("\tterminals\n");
    SEPARATOR;
    while(NULL != (elem = iterate_term_list(&mark)))
        PRINT("\t%3d. %s\n", mark, elem->type_str->buffer);
    PRINT("\n");

}
