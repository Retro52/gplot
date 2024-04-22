cmake_minimum_required(VERSION 3.21)
project(gplot-showcase)

set(CMAKE_CXX_STANDARD 20)

include(Utils)
setup_custom_bin_output("${CMAKE_SOURCE_DIR}/bin.${CMAKE_BUILD_TYPE}")

list(APPEND PROJECT_INCLUDES "${CMAKE_SOURCE_DIR}/vendors")
file(GLOB_RECURSE PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/src/*" "${PROJECT_SOURCE_DIR}/include/*")

list(APPEND PROJECT_LINK_LIBS imgui)
list(APPEND PROJECT_LINK_LIBS SDL2::SDL2)
list(APPEND PROJECT_LINK_LIBS SDL2::SDL2main)
list(APPEND PROJECT_LINK_LIBS gplot::gplot-core)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LINK_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
