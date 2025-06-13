project(table_pass1)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_files}
)

