/**
 * @file render.c
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-12
 * @copyright Copyright (c) 2025
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lists.h"
#include "render_init.h"
#include "strgs.h"
#include "templates.h"

static void rend_file(const char* buf, const char* name) {

    char buffer[1024];
    FILE* fp = fopen(make_fname(buffer, sizeof(buffer), name), "w");

    render_template(buf, fp, render_table);
    fprintf(fp, "\n\n");
    fclose(fp);
}

static void set_project_name(const char* str) {

    if(master_list->project_name != NULL) {
        clear_string(master_list->project_name);
        append_string(master_list->project_name, str);
    }
    else
        master_list->project_name = create_string(str);
}

void render(void) {

    render_init();

    set_project_name("ast");
    rend_file(ast_h_template, "ast/ast.h");
    rend_file(ast_protos_h_template, "ast/ast_protos.h");
    rend_file(ast_c_template, "ast/ast.c");
    rend_file(cmake_template, "ast/CMakeLists.txt");

    set_project_name("parser");
    rend_file(parser_h_template, "parser/parser.h");
    rend_file(parser_protos_h_template, "parser/parser_protos.h");
    rend_file(parser_c_template, "parser/parser.c");
    rend_file(cmake_template, "parser/CMakeLists.txt");

    set_project_name("scanner");
    rend_file(tokens_h_template, "scanner/tokens.h");
    rend_file(tokens_c_template, "scanner/tokens.c");
    rend_file(scanner_h_template, "scanner/scanner.h");
    rend_file(scanner_l_template, "scanner/scanner.l");
    rend_file(cmake_l_template, "scanner/CMakeLists.txt");
    // rend_file(cmake_template, "scanner/CMakeLists.txt");
}
