cmake_minimum_required(VERSION 3.14)
project(Stego C)

set(CMAKE_C_STANDARD 11)

#include_directories(/Users/fer/Downloads/Workspace/Stego)

add_executable(Stego main.c libbmp.c)