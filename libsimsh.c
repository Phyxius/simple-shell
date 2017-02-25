//
// Created by Shea on 2017-02-25.
//

#include <stdio.h>
#include <string.h>
#include "libsimsh.h"
#include "aux_files/chop_line.h"

void print_prompt()
{
    printf("simsh: ");
}

validation_t validate_line(const chopped_line_t *line)
{
    for (int i = 0; i < line->num_tokens - 1; ++i)
    {
        if (strcmp(line->tokens[i], "&") == 0) return BAD_AMPERSAND;
    }
    return VALID;
}

const char* get_validation_result(validation_t result)
{
    switch(result)
    {
        case VALID: return "Valid";
        case BAD_AMPERSAND: return "Out-of-place ampersand";
        default: return NULL;
    }
}
