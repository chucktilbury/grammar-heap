project(common)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    #${${PROJECT_NAME}_files}
    alloc.c
    fileio.c
    hash.c
    ptr_list.c
    strg_list.c
    strgs.c
    trace.c
    cmdline.c
)

