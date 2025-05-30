#ifndef _RENDER_TEMPLATE_H_
#define _RENDER_TEMPLATE_H_

#include <stdio.h>
#include "common.h"

typedef struct {
    const char* name;
    void (*render_func)(FILE*);
} render_item_t;

typedef hash_table_t render_table_t;

render_table_t* create_render_table(void);
render_item_t* create_render_item(const char* name, void (*render_func)(FILE*));
void add_render(render_table_t* ptr, render_item_t* item);
void render_template(const char* template, FILE* outf, render_table_t* ptr);

#endif /* _RENDER_TEMPLATE_H_ */
