cmake_minimum_required(VERSION 3.26)
project(gplot)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(SDL_STATIC_ENABLED_BY_DEFAULT ON)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

add_subdirectory(vendors/sdl)
add_subdirectory(vendors/glad)
add_subdirectory(vendors/imgui)

add_subdirectory(gplot-core)
add_subdirectory(gplot-showcase)
