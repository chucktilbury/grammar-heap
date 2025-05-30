/**
 * @file render_init.h
 *
 * @brief Init the render data structures.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-04-10
 * @copyright Copyright (c) 2025
 */
#ifndef _RENDER_INIT_H_
#define _RENDER_INIT_H_

#include "render_template.h"

extern render_table_t* render_table;

void render_init(void);
void make_render_dirs(void);
const char* make_fname(char* buf, size_t size, const char* base);
const char* make_ast_fname(const char* base);
const char* make_parser_fname(const char* base);
const char* make_scanner_fname(const char* base);

#endif /* _RENDER_INIT_H_ */
