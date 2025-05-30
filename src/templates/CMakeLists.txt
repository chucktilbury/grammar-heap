project(templates)

add_executable(build build.c)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/templates.c
    COMMAND build ${PROJECT_SOURCE_DIR}
    DEPENDS build
)

add_library(${PROJECT_NAME} STATIC
    templates.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}/../common
)

