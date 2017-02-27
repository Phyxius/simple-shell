//
// Created by Shea on 2017-02-25.
//

#ifndef LAB_2_LIBSIMSH_H
#define LAB_2_LIBSIMSH_H
#include <stdbool.h>
#include "aux_files/chop_line.h"
#include "aux_files/list.h"

typedef enum validation_t {
    VALID = 0,
    BAD_AMPERSAND,
    AMBIGUOUS_INPUT_REDIRECT,
    AMBIGUOUS_OUTPUT_REDIRECT,
    MISSING_REDIRECT_NAME,
    NULL_COMMAND,
} validation_t;

typedef struct command_t {
    char ** args;
    bool create_output;
    char * output_file;
    char * input_file;
    bool foreground;
} command_t;

bool streq(const char * str1, const char * str2);
void print_prompt();
validation_t validate_line(const chopped_line_t *line);
const char* get_validation_result(validation_t result);
bool launch_process(char * const * args, bool wait, int in_fd, int out_fd); //true on success
void setup_sigchld_handler();
command_t parse_chopped_line(chopped_line_t *line);
void free_command_t(command_t * command);
#endif //LAB_2_LIBSIMSH_H
