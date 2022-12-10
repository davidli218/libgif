cmake_minimum_required(VERSION 3.24)
project(libgif_proj C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(src)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(libgif_demo demo.c)

target_link_libraries(libgif_demo libgif)
