project(lists)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    #${${PROJECT_NAME}_files}
    lists.c
    nterm_comment.c
    nterm_ds.c
    nterm_list.c
    nterm_rules.c
    term_list.c
)

