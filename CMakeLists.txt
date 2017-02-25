cmake_minimum_required(VERSION 3.6)
project(Lab_2)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -D_GNU_SOURCE")

set(SOURCE_FILES simsh.c aux_files/chop_line.h aux_files/chop_line.c aux_files/list.h aux_files/list.c libsimsh.h libsimsh.c)
add_executable(simsh ${SOURCE_FILES})