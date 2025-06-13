project(table_pass2)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_files}
)

