
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "nterm_list.h"
#include "ptr_list.h"
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "common.h"
#include "lists.h"
#include "render.h"
#include "strg_list.h"
#include "strgs.h"
#include "nterm_rules.h"
#include "generate.h"

extern master_list_t* master_list;

void print_terminal_list(void) {

    int mark = 0;
    int count = 1;

    printf("\n-----------------------------\n");
    printf("     Terminal List\n");
    printf("-----------------------------\n");

    term_item_t* term;
    while(NULL != (term = iterate_term_list(master_list->term_list, &mark)))
        printf("%3d. %-25s%s\n", count++, raw_string(term->term), raw_string(term->token));
}

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "Use: %s filename\n", argv[0]);
        exit(1);
    }

    yyin = fopen(argv[1], "r");
    if(yyin == NULL) {
        fprintf(stderr, "cannot open input file \"%s\": %s", argv[1], strerror(errno));
        exit(1);
    }

    yyparse();

    make_raw_lists();
    generate_rules();
    render();

    // int mark = 0;
    // nterm_item_t* node;
    //
    // while(NULL != (node = iterate_ptr_list(master_list->nterm_list, &mark))) {
    //     int point = 0;
    //     string_t* str;
    //     emit_string_fmt(stdout, "\n// rule_name: %s\n", node->nterm->buffer);
    //     while(NULL != (str = iterate_string_list(node->rule_comment, &point)))
    //         emit_string(stdout, str);
    // }
    //
    // mark = 0;
    // while(NULL != (node = iterate_ptr_list(master_list->nterm_list, &mark))) {
    //     int point = 0;
    //     rule_state_t* rule;
    //     emit_string_fmt(stdout, "\n// rule_name: %s\n", node->nterm->buffer);
    //     while(NULL != (rule = iterate_ptr_list(node->rule_states, &point))) {
    //         int thing = 0;
    //         string_t* str;
    //         while(NULL != (str = iterate_string_list(rule->out, &thing)))
    //             emit_string(stdout, str);
    //     }
    // }


    return 0;
}
