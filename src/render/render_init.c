/**
 * @file render_init.c
 *
 * @brief Init the render data structures
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-10
 * @copyright Copyright (c) 2025
 */
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "strg_list.h"
#include "strgs.h"
#include "term_list.h"
#include "nterm_list.h"
#include "templates.h"

#include "render_init.h"
#include "render_parser.h"
#include "lists.h"

#define DEV_NAME "Chuck Tilbury"

render_table_t* render_table = NULL;

static void _ctime(FILE* fp) {

    time_t t = time(NULL);
    char* tpt = ctime(&t);
    tpt[strlen(tpt) - 1] = '\0';
    fputs(tpt, fp);
}

static void _dev_name(FILE* fp) {

    fprintf(fp, DEV_NAME);
}

static void _file_name(FILE* fp) {

    fprintf(fp, "%s", master_list->current_file->buffer);
}

static void _file_header(FILE* fp) {

    render_template(file_header_template, fp, render_table);
}

static void _first_node(FILE* fp) {

    nterm_item_t* item = master_list->first_nterm;
    fprintf(fp, "%s", item->nterm->buffer);
}

static void _project_name(FILE* fp) {

    fprintf(fp, "%s", master_list->project_name->buffer);
}

static void _type_to_str(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        render_template(ast_type_to_str_template, fp, render_table);
        // fprintf(fp, "\n");
    }
}

static void _type_to_size(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        render_template(ast_type_to_size_template, fp, render_table);
        // fprintf(fp, "\n");
    }
}

static void _type_list(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;
    int val = 512;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        fprintf(fp, "    %s = %d", item->type->buffer, val);
        if((size_t)mark + 1 <= master_list->nterm_list->len)
            fprintf(fp, ",\n");
        val++;
    }
}

static void _data_structures(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        render_template(ast_data_struct_template, fp, render_table);
        fprintf(fp, "\n");
    }
}

static void _ast_func_protos(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        render_template(ast_func_proto_template, fp, render_table);
        // fprintf(fp, "\n");
    }
}

static void _parser_func_protos(FILE* fp) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        render_template(parser_func_proto_template, fp, render_table);
        // fprintf(fp, "\n");
    }
}

static void _nterm_name(FILE* fp) {

    nterm_item_t* item = master_list->crnt_nterm; // index_nterm_list(master_list->nterm_list, master_list->nterm_idx);
    fprintf(fp, "%s", item->nterm->buffer);
}

static void _type_name(FILE* fp) {

    nterm_item_t* item = master_list->crnt_nterm; // index_nterm_list(master_list->nterm_list, master_list->nterm_idx);
    fprintf(fp, "%s", item->type->buffer);
}

static void _token_type_list(FILE* fp) {

    int mark = 0;
    term_item_t* item;
    int val = 258;

    while(NULL != (item = iterate_term_list(master_list->term_list, &mark))) {
        fprintf(fp, "    %s = %d", item->token->buffer, val);
        if((size_t)mark + 1 <= master_list->term_list->len)
            fprintf(fp, ",\n");
        val++;
    }
}

static void _tok_type_to_str(FILE* fp) {

    int mark = 0;
    term_item_t* item;

    while(NULL != (item = iterate_term_list(master_list->term_list, &mark))) {
        fprintf(fp, "        (tok->type == %s)? \"%s\":\n", item->token->buffer, item->term->buffer);
    }
}

static void _ast_struct_elements(FILE* fp) {

    nterm_item_t* nterm = master_list->crnt_nterm; // index_nterm_list(master_list->nterm_list, master_list->nterm_idx);
    nterm_ds_type_t* item;
    int mark = 0;

    while(NULL != (item = iterate_ptr_list(nterm->ds_names, &mark))) {
        if(item->type) {
            // terminal symbol
            fprintf(fp, "    // token_t* %s;\n", item->name->buffer);
        }
        else {
            // non-terminal
            fprintf(fp, "    // struct _ast_%s_t_* %s;\n", item->name->buffer, item->name->buffer);
        }
    }
}

static void _parser_struct_elements(FILE* fp) {

    nterm_item_t* nterm = master_list->crnt_nterm; // index_nterm_list(master_list->nterm_list, master_list->nterm_idx);
    nterm_ds_type_t* item;
    int mark = 0;

    while(NULL != (item = iterate_ptr_list(nterm->ds_names, &mark))) {
        if(item->type) {
            // terminal symbol
            fprintf(fp, "// token_t* %s = NULL;\n", item->name->buffer);
        }
        else {
            // non-terminal
            fprintf(fp, "// ast_%s_t* %s = NULL;\n", item->name->buffer, item->name->buffer);
        }
    }
}

static void _parser_assignment_elements(FILE* fp) {

    nterm_item_t* nterm = master_list->crnt_nterm; // index_nterm_list(master_list->nterm_list, master_list->nterm_idx);
    nterm_ds_type_t* item;
    int mark = 0;

    while(NULL != (item = iterate_ptr_list(nterm->ds_names, &mark))) {
        fprintf(fp, "// retv->%s = %s;\n", item->name->buffer, item->name->buffer);
    }
}

static void _ast_func_implementation(FILE* fp) {

    fprintf(fp, "// ast implementation is TBD\n");
}

void dump_rule(FILE* fp, nterm_item_t* rule);
static void _dump_rule(FILE* fp) {

    dump_rule(fp, master_list->crnt_nterm); // index_nterm_list(master_list->nterm_list, master_list->nterm_idx));
}

static void _gen_scanner_rules(FILE* fp) {

    term_item_t* term;
    int mark = 0;

    while(NULL != (term = iterate_term_list(master_list->term_list, &mark))) {
        if(term->type) {
            fprintf(fp, "\"%s\"\t{\n", term->term->buffer);
            fprintf(fp, "    add_token_queue(create_token(create_string(yytext), %s));\n", term->token->buffer);
            fprintf(fp, "    return %s;\n}\n\n", term->token->buffer);
        }
    }
}

static void _render_parser_comment(FILE* fp) {

    int mark = 0;
    nterm_item_t* ptr = master_list->crnt_nterm; // master_list->nterm_list->buffer[master_list->nterm_idx];
    string_t* s;
    // printf("\n%s\n", ptr->nterm->buffer);
    while(NULL != (s = iterate_string_list(ptr->rule_comment, &mark)))
        emit_string(fp, s);
}

static void make_render_table(void) {

    render_table = create_render_table();

    add_render(render_table, create_render_item("ctime", _ctime));
    add_render(render_table, create_render_item("dev_name", _dev_name));
    add_render(render_table, create_render_item("file_name", _file_name));
    add_render(render_table, create_render_item("file_header", _file_header));

    add_render(render_table, create_render_item("ast_type_list", _type_list));
    add_render(render_table, create_render_item("ast_data_structures", _data_structures));
    add_render(render_table, create_render_item("ast_function_protos", _ast_func_protos));
    add_render(render_table, create_render_item("ast_type_name", _type_name));
    add_render(render_table, create_render_item("ast_nterm_name", _nterm_name));
    add_render(render_table, create_render_item("ast_type_to_size", _type_to_size));
    add_render(render_table, create_render_item("ast_type_to_str", _type_to_str));
    add_render(render_table, create_render_item("ast_first_node", _first_node));
    add_render(render_table, create_render_item("ast_func_implementation", _ast_func_implementation));

    add_render(render_table, create_render_item("ast_struct_elements", _ast_struct_elements));
    add_render(render_table, create_render_item("data_structures", _parser_struct_elements));
    add_render(render_table, create_render_item("assign_data_structures", _parser_assignment_elements));

    add_render(render_table, create_render_item("parser_protos", _parser_func_protos));
    add_render(render_table, create_render_item("parse_func_implementation", emit_parser_implementation));
    add_render(render_table, create_render_item("parse_func_comment", _render_parser_comment));

    add_render(render_table, create_render_item("token_type_list", _token_type_list));
    add_render(render_table, create_render_item("tok_type_to_str", _tok_type_to_str));
    add_render(render_table, create_render_item("gen_scanner_rules", _gen_scanner_rules));

    add_render(render_table, create_render_item("dump_rule", _dump_rule));
    add_render(render_table, create_render_item("project_name", _project_name));
}


// NOTE that these values will be supplied on the command line and have
// reasonable defaults but for now, they are hard-coded.
void make_render_dirs(void) {

    // remove("./out");
    system("rm -rf ./out/");
    mkdir("./out", 0777);
    mkdir("./out/ast", 0777);
    mkdir("./out/parser", 0777);
    mkdir("./out/scanner", 0777);
}

const char* make_fname(char* buf, size_t size, const char* base) {

    snprintf(buf, size, "./out/%s", base);
    if(master_list->current_file != NULL)
        destroy_string(master_list->current_file);
    master_list->current_file = create_string(buf);
    return master_list->current_file->buffer;
}

const char* make_ast_fname(const char* base) {

    // snprintf(buf, size, "./out/ast/%s", base);
    if(master_list->current_file == NULL)
        master_list->current_file = create_string_fmt("./out/ast/%s.c", base);
    else {
        clear_string(master_list->current_file);
        append_string_fmt(master_list->current_file, "./out/ast/%s.c", base);
    }
    // master_list->current_file = create_string(buf);
    return master_list->current_file->buffer;
}

const char* make_parser_fname(const char* base) {

    if(master_list->current_file == NULL)
        master_list->current_file = create_string_fmt("./out/parser/%s.c", base);
    else {
        clear_string(master_list->current_file);
        append_string_fmt(master_list->current_file, "./out/parser/%s.c", base);
    }
    return master_list->current_file->buffer;
}

const char* make_scanner_fname(const char* base) {

    if(master_list->current_file == NULL)
        master_list->current_file = create_string_fmt("./out/scanner/%s.c", base);
    else {
        clear_string(master_list->current_file);
        append_string_fmt(master_list->current_file, "./out/scanner/%s.c", base);
    }
    return master_list->current_file->buffer;
}

void render_parser_funcs(void) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        FILE* fp = fopen(make_parser_fname(item->nterm->buffer), "w");
        render_template(parser_func_def_template, fp, render_table);
        fprintf(fp, "\n");
        fclose(fp);
    }
}

static void render_ast_funcs(void) {

    int mark = 0;
    nterm_item_t* item;

    while(NULL != (item = iterate_nterm_list(master_list->nterm_list, &mark))) {
        master_list->crnt_nterm = item;
        FILE* fp = fopen(make_ast_fname(item->nterm->buffer), "w");
        render_template(ast_func_def_template, fp, render_table);
        fprintf(fp, "\n");
        fclose(fp);
    }
}

void render_init(void) {

    make_render_dirs();
    make_render_table();
    render_parser_funcs();
    render_ast_funcs();
}
