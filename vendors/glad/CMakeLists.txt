cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(glad C CXX)

list(APPEND GLAD_SOURCE_DIRS "${PROJECT_SOURCE_DIR}/src")
list(APPEND GLAD_SOURCE_DIRS "${PROJECT_SOURCE_DIR}/include")

collect_files_recursively("${GLAD_SOURCE_DIRS}" "GLAD_SOURCES")

list(APPEND GLAD_INCLUDES "${PROJECT_SOURCE_DIR}/include")

add_library(${PROJECT_NAME} STATIC ${GLAD_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDES})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "extern")
append_target_suffix("${PROJECT_NAME}" "${TARGET_NAME_SUFFIX}")
generate_filter_tree("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")
