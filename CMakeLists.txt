cmake_minimum_required(VERSION 3.22)
project(libbmp C)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_subdirectory(src)

add_subdirectory(example)
