
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ast.h"
#include "scanner.h"
#include "parser.h"
//#include "ast.h"
#include "cmdline.h"
#include "lists.h"

void cmdline(int argc, char** argv) {

    init_cmdline("heap", "FSA parser generator", "0.1");
    add_cmdline('h', "help", NULL, "print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "file name to input", NULL, NULL, CMD_REQD|CMD_ANON);

    parse_cmdline(argc, argv);
}

int main(int argc, char** argv) {

    cmdline(argc, argv);

    yyin = fopen(raw_string(get_cmd_opt("files")), "r");
    if(yyin == NULL) {
        fprintf(stderr, "cannot open input file \"%s\": %s\n", argv[1], strerror(errno));
        cmdline_help();
    }

    yyparse();

    //make_raw_lists();
    traverse_ast(root_node);

    return 0;
}
