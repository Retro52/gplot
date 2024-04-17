cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(glad C CXX)

list(APPEND GLAD_INCLUDES "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE GLAD_SOURCES "${PROJECT_SOURCE_DIR}/src/*")

add_library(${PROJECT_NAME} STATIC ${GLAD_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLAD_INCLUDES})
