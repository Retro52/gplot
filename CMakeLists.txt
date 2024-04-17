cmake_minimum_required(VERSION 3.26)
project(gplot)

set(CMAKE_CXX_STANDARD 20)

set(SDL_TEST OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

add_subdirectory(vendors/sdl)
add_subdirectory(vendors/glad)

file(GLOB_RECURSE PROJECT_SOURCES src/*)

list(APPEND PROJECT_INCLUDES "${CMAKE_SOURCE_DIR}/src")
list(APPEND PROJECT_INCLUDES "${CMAKE_SOURCE_DIR}/vendors")

list(APPEND PROJECT_LINK_LIBS glad)
list(APPEND PROJECT_LINK_LIBS SDL2::SDL2)
list(APPEND PROJECT_LINK_LIBS SDL2::SDL2main)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LINK_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
