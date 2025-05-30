project(render)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    #${${PROJECT_NAME}_files}
    render.c
    render_init.c
    render_rule.c
    render_template.c
    render_parser.c
)

