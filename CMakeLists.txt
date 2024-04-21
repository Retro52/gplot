cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
project(gplot)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(PlatformSetup)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(SDL_STATIC_ENABLED_BY_DEFAULT ON)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

add_subdirectory(vendors/sdl)
add_subdirectory(vendors/glad)
add_subdirectory(vendors/imgui)

add_subdirectory(gplot-core)
add_subdirectory(gplot-showcase)
