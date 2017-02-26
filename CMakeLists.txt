cmake_minimum_required(VERSION 3.6)
project(Lab_2)

add_compile_options(-Wall -Werror -g -D_GNU_SOURCE -std=c11)

set(SOURCE_FILES simsh.c aux_files/chop_line.h aux_files/chop_line.c aux_files/list.h aux_files/list.c libsimsh.h libsimsh.c)
add_executable(simsh ${SOURCE_FILES})