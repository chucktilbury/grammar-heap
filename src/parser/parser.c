
#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"
#include "tokens.h"
#include "parser.h"
#include "array.h"

static int errors = 0;
static parser_state_t* parser_state;

#define DUMP(r) \
    do { \
        fprintf(stderr, "rule: %s: ", (r)->name->str->buffer); \
        token_t* t; \
        int mark = 0; \
        while(NULL != (t = iterate_ptr_list((r)->expr, &mark))) \
            fprintf(stderr, "%s ", t->str->buffer); \
        fprintf(stderr, "\n"); \
    } while(0)

// shunting yard to convert infix expression token stream to postfix.
// see https://swtch.com/~rsc/regexp/regexp1.html
static pointer_list_t* expression(void) {

    int finished = 0;
    token_t* tok;
    pointer_list_t* out = create_ptr_list();
    pointer_list_t* stack = create_ptr_list();

    // tracking parentheses depth
    struct parns_t {
        int num_alts;  // number of alternates that have been seen
        int num_atoms; // number of atomic operands (not operators)
    } parn, *p_parn;

    array_t* parn_stack = create_array(sizeof(struct parns_t), NULL);
    int num_alts = 0;
    int num_atoms = 0;

    while(! finished) {
        tok = get_token();

        if(tok == NULL) {
            fprintf(stderr, "syntax error: unexpected end of input\n");
            errors++;
            return NULL; // no match
        }

        switch(tok->type) {
            case PRETEXT:
            case POSTTEXT:
            case PRECODE:
            case POSTCODE:
            case TERM_DEF:
            case NTERM_DEF:
            case PROVIDES:
            case REQUIRES:
                fprintf(stderr, "syntax error: %d: unexpected directive \"%s\"\n", tok->line_no, tok->str->buffer);
                errors++;
                return NULL; // no match

            case COLON:
                fprintf(stderr, "syntax error: %d: unexpected \":\" encountered\n", tok->line_no);
                errors++;
                return NULL;

            case PLUS:
            case STAR:
            case QUESTION:
                if(num_atoms == 0) {
                    fprintf(stderr, "syntax error: %d: unexpected '%s' encountered\n", tok->line_no, tok->str->buffer);
                    errors++;
                    return NULL;
                }

                append_ptr_list(out, copy_token(tok));
                consume_token(); // consume the operator
                break;

            case PIPE:
                if(num_atoms == 0) {
                    fprintf(stderr, "syntax error: %d: unexpected '|' encountered\n", tok->line_no);
                    errors++;
                    return NULL;
                }

                while(--num_atoms > 0) {
                    append_ptr_list(out, create_token(".", CATENATE));
                }

                num_alts++;

                consume_token(); // consume the '|'
                break;

            case OPAREN:
                if(num_atoms > 1) {
                    num_atoms--;
                    append_ptr_list(out, create_token(".", CATENATE));
                }

                parn.num_alts = num_alts;
                parn.num_atoms = num_atoms;
                push_array(parn_stack, &parn);
                num_alts = num_atoms = 0;
                parn.num_atoms = 0;

                consume_token(); // consume the '(' token
                break;

            case CPAREN:
                if(NULL == peek_array(parn_stack)) {
                    fprintf(stderr, "syntax error: %d: unexpected ')' encountered (eps)\n", tok->line_no);
                    errors++;
                    return NULL;
                }

                if(num_atoms == 0) {
                    fprintf(stderr, "syntax error: %d: unexpected ')' encountered (n0)\n", tok->line_no);
                    errors++;
                    return NULL;
                }

                while(--num_atoms > 0) {
                    append_ptr_list(out, create_token(".", CATENATE));
                }

                for(; num_alts > 0; num_alts--) {
                    append_ptr_list(out, create_token("|", PIPE));
                }

                p_parn = pop_array(parn_stack);
                num_alts = p_parn->num_alts;
                num_atoms = p_parn->num_atoms + 1;

                consume_token();     // consume the ')'
                break;

            case CODE_BLOCK:
            case NON_TERMINAL:
            case TERMINAL_SYMBOL:
            case TERMINAL_KEYWORD:
            case TERMINAL_OPER:
                if(num_atoms > 1) {
                    --num_atoms;
                    append_ptr_list(out, create_token(".", CATENATE));
                }

                append_ptr_list(out, copy_token(tok));
                num_atoms++;

                consume_token(); // consume the operand
                break;

            case SEMICOLON:
{
                if(NULL != peek_array(parn_stack)) {
                    fprintf(stderr, "syntax error: %d: imbalanced parentheses\n", tok->line_no);
                    errors++;
                    return NULL;
                }

                while(--num_atoms > 0) {
                    append_ptr_list(out, create_token(".", CATENATE));
                }

                for(; num_alts > 0; num_alts--) {
                    append_ptr_list(out, create_token("|", PIPE));
                }

                destroy_array(parn_stack);
                destroy_ptr_list(stack);
                finished++; }
                // do not consume the semicolon
                break;

            default:
                fprintf(stderr, "internal error: unknown token type in expression(): %d", tok->type);
                exit(1);
        }
    }

    return out;
}

static int rule(void) {

    int finished = 0;
    int state = 0;
    int result = 0; // no match
    token_t* tok;

    token_t* name = NULL;
    rule_t* rule;
    pointer_list_t* expr = NULL;

    while(! finished) {
        tok = get_token();
        switch(state) {
            case 0:
                // non-terminal or no match
                if(tok->type == NON_TERMINAL) {
                    name = copy_token(tok);
                    consume_token();
                    state = 1;
                }
                else
                    state = 101; // no match no error
                break;

            case 1:
                // COLON or error
                if(tok->type == COLON) {
                    consume_token();
                    state = 2;
                }
                else {
                    fprintf(stderr, "syntax error: %d: expected a \":\" but got a \"%s\"\n", tok->line_no, tok->str->buffer);
                    errors++;
                    state = 102;
                }
                break;

            case 2:
                // expression or error
                if(NULL != (expr = expression()))
                    state = 3;
                else
                    state = 102;
                break;

            case 3:
                // SEMICOLON or error
                if(tok->type == SEMICOLON) {
                    consume_token();
                    state = 100;
                }
                else {
                    fprintf(stderr, "syntax error: %d: expected a \";\" but got a \"%s\"\n", tok->line_no, tok->str->buffer);
                    errors++;
                    state = 102;
                }
                break;

            case 100:
                // match
                rule = create_rule(name);
                parser_state->crnt_rule = rule;
                parser_state->crnt_rule->expr = expr;

                if(parser_state->start_rule == NULL)
                    parser_state->start_rule = rule;

                append_ptr_list(parser_state->rule_list, rule);

                DUMP(rule);
                result++;
                finished++;
                break;

            case 101:
                // no match, no error
                finished++;
                break;

            case 102:
                // syntax error
                finished++;
                break;
        }
    }

    return result;
}

static int directive(void) {

    int finished = 0;
    int state = 0;
    int result = 0; // no match
    token_t* tok;

    while(! finished) {
        tok = get_token();
        switch(state) {
            case 0:
                // directive or no match
                switch(tok->type) {
                    case PRETEXT: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->pretext = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%pretext\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case POSTTEXT: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->posttext = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%posttext\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case PRECODE: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->precode = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%precode\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case POSTCODE: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->postcode = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%postcode\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case TERM_DEF: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->term_def = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%term_def\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case NTERM_DEF: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->nterm_def = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%nterm_def\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case PROVIDES: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->provides = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%provides\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    case REQUIRES: {
                        token_t* cb = consume_token();
                        if(cb->type == CODE_BLOCK) {
                            parser_state->requires = copy_string(cb->str);
                            state = 100;
                        }
                        else {
                            fprintf(stderr, "syntax error: %d: expected code block after \"%%requires\" directive\n", tok->line_no);
                            fprintf(stderr, "\tbut got %s\n", tok_to_str(cb->type));
                            errors++;
                            state = 102;
                        }
                    } break;
                    default:
                        state = 101;
                        break;
                }
                break;

            case 100:
                // match
                finished++;
                result++;
                break;

            case 101:
                // no match, no error
                finished++;
                break;

            case 102:
                // syntax error
                finished++;
                break;
        }
    }

    return result;
}

int parser(void) {

    int finished = 0;
    int state = 0;
    token_t* tok;

    while(! finished) {
        tok = get_token();

        // end of file
        if(tok == NULL)
            finished++;
        else if(errors != 0)
            finished++;
        // rule or directive
        else if(rule())
            ; // found a rule
        else if(directive())
            ; // found a directive
        else {
            fprintf(stderr, "syntax error: %d: expected a rule, a directive, or end of the file\n", tok->line_no);
            fprintf(stderr, "\tbut got %s\n", tok_to_str(tok->type));
            errors++;
        }
    }

    return errors;
}

void init_parser(void) {

    parser_state = _ALLOC_TYPE(parser_state_t);
    parser_state->pretext = create_string(NULL);
    parser_state->posttext = create_string(NULL);
    parser_state->precode = create_string(NULL);
    parser_state->postcode = create_string(NULL);
    parser_state->term_def = create_string(NULL);
    parser_state->nterm_def = create_string(NULL);
    parser_state->provides = create_string(NULL);
    parser_state->requires = create_string(NULL);
    parser_state->rule_list = create_ptr_list();
    parser_state->start_rule = NULL;

    init_scanner();
    consume_token();
}

rule_t* create_rule(token_t* name) {

    rule_t* ptr = _ALLOC_TYPE(rule_t);
    ptr->name = name;
    ptr->expr = NULL;

    return ptr;
}


