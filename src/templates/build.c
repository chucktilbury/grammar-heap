/**
 * @file build.c
 *
 * @brief Compile the templates into lists of strings.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-11
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "errors.h"

#define HEADER "templates.h"
#define SOURCE "templates.c"

static const char* template_list[] = {
    "ast_h",
    "ast_protos_h",
    "ast_c",
    "ast_data_struct",
    "ast_func_proto",
    "ast_func_def",
    "ast_type_to_size",
    "ast_type_to_str",
    "parser_c",
    "parser_h",
    "parser_protos_h",
    "parser_func_def",
    "parser_func_proto",
    "file_header",
    "tokens_h",
    "tokens_c",
    "scanner_h",
    "cmake",
    "scanner_l",
    "cmake_l",
    NULL
};

static const char* intro =
        "/*\n"
        " * GENERTATED FILE. DO NOT EDIT.\n"
        " */\n\n";

static char template_dir[1024 * 4];

static void gen_header(void) {

    FILE* fp = fopen(HEADER, "w");
    if(fp == NULL)
        FATAL("cannot open output file \"%s\": %s\n", HEADER, strerror(errno));

    fputs(intro, fp);
    fputs("#ifndef _TEMPLATES_H_\n#define _TEMPLATES_H_\n\n", fp);
    for(int i = 0; template_list[i] != NULL; i++) {
        fprintf(fp, "extern const char* %s_template;\n", template_list[i]);
    }

    fputs("\n#endif /* _TEMPLATES_H_ */\n\n", fp);
}

static void gen_source(void) {

    char buf[1024];
    FILE* inf;
    FILE* fp = fopen(SOURCE, "w");
    if(fp == NULL)
        FATAL("cannot open output file \"%s\": %s\n", SOURCE, strerror(errno));

    fputs(intro, fp);

    for(int i = 0; template_list[i] != NULL; i++) {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, template_dir, sizeof(buf));
        strncat(buf, "/", sizeof(buf) - strlen(buf) - 1);
        strncat(buf, template_list[i], sizeof(buf) - strlen(buf) - 1);
        strncat(buf, ".txt", sizeof(buf) - strlen(buf) - 1);
        inf = fopen(buf, "r");
        if(inf == NULL)
            FATAL("cannot open input file: \"%s\": %s\n", buf, strerror(errno));

        fprintf(fp, "const char* %s_template = \n", template_list[i]);
        memset(buf, 0, sizeof(buf));
        while(NULL != fgets(buf, sizeof(buf), inf)) {
            char* tpt = strrchr(buf, '\n');
            if(tpt != NULL)
                *tpt = '\0';

            fprintf(fp, "\"");
            for(int j = 0; buf[j] != '\0'; j++) {
                if(buf[j] == '\"')
                    fputs("\\\"", fp);
                else if(buf[j] == '\\')
                    fputs("\\\\", fp);
                else
                    fputc(buf[j], fp);
            }
            fprintf(fp, "\\n\" \\\n");
            memset(buf, 0, sizeof(buf));
        }

        fputs(";\n\n", fp);
    }
}

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "Use: %s template_dir\n", argv[0]);
        exit(1);
    }

    strncpy(template_dir, argv[1], sizeof(template_dir));
    // fprintf(stderr, "%s\n", template_dir);

    gen_header();
    gen_source();
    return 0;
}
