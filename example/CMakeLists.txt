cmake_minimum_required(VERSION 3.22)
project(libgif_demo C)

set(CMAKE_C_STANDARD 11)

add_executable(libbmp_demo demo.c)

target_link_libraries(libbmp_demo libbmp)
