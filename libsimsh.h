//
// Created by Shea on 2017-02-25.
//

#ifndef LAB_2_LIBSIMSH_H
#define LAB_2_LIBSIMSH_H
#include <stdbool.h>
#include "aux_files/chop_line.h"
#include "aux_files/list.h"

typedef enum validation_t { VALID = 0, BAD_AMPERSAND} validation_t;

void print_prompt();
validation_t validate_line(const chopped_line_t *line);
const char* get_validation_result(validation_t result);
bool launch_process(char * const * args, bool wait, int in_fd, int out_fd); //true on success
void setup_sigchld_handler();
#endif //LAB_2_LIBSIMSH_H
