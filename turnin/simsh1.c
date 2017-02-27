#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <string.h>
#include "libsimsh1.h"


int main()
{
    setup_sigchld_handler();
    char* line = NULL;
    size_t lineLength = 0;
    ssize_t read;
    print_prompt();
    while((read = getline(&line, &lineLength, stdin)) != -1)
    {
        if (line[read - 1] == '\n') line[read -1] = '\0';
        chopped_line_t * chopped_line = get_chopped_line(line);
        if (chopped_line->num_tokens < 1)
        {
            free_chopped_line(chopped_line);
            print_prompt();
            continue;
        }
        else if (strcmp(chopped_line->tokens[0], "exit") == 0)
        {
            free_chopped_line(chopped_line);
            errno = 0;
            break;
        }
        validation_t validation_result = validate_line(chopped_line);
        if (validation_result != VALID)
        {
            fprintf(stderr, "Error: %s\n", get_validation_result(validation_result));
        }
        else
        {
            bool foreground = strcmp(chopped_line->tokens[chopped_line->num_tokens - 1], "&") != 0;
            if (!launch_process(chopped_line, foreground))
            {
                perror("Error");
            }
        }
        free_chopped_line(chopped_line);
        print_prompt();
    }
    free(line);
    if (!feof(stdin) && errno != 0)
    {
        perror("getline");
        exit(EXIT_FAILURE);
    }
    while (wait(NULL)) { //wait for all children to exit
        if (errno == ECHILD) {
            break;
        }
    }
    return EXIT_SUCCESS;
}