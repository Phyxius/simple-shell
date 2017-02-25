//
// Created by Shea on 2017-02-25.
//

#ifndef LAB_2_LIBSIMSH_H
#define LAB_2_LIBSIMSH_H
#include <stdbool.h>
#include "aux_files/chop_line.h"
#include "aux_files/list.h"

void print_prompt();
bool is_valid_line(const chopped_line_t * line);
#endif //LAB_2_LIBSIMSH_H
