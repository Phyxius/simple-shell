#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <string.h>
#include <zconf.h>
#include <sys/fcntl.h>
#include "libsimsh.h"

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
        else if (streq(chopped_line->tokens[0], "exit"))
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
            command_t command = parse_chopped_line(chopped_line);
            bool error = false;
            int in_fd = STDIN_FILENO;
            int out_fd = STDOUT_FILENO;
            if (command.input_file)
            {
                in_fd = open(command.input_file, O_RDONLY);
                if (in_fd < 0)
                {
                    error = true;
                    perror(command.input_file);
                }
            }
            if (!error && command.output_file)
            {
                int flags = O_WRONLY;
                if (command.create_output) flags |= O_CREAT | O_EXCL;
                else flags |= O_APPEND;
                out_fd = open(command.output_file, flags, 0666);
                if (out_fd < 0)
                {
                    error = true;
                    perror(command.output_file);
                }
            }
            if (!error && !launch_process(command.args, command.foreground, in_fd, out_fd))
            {
                perror("Error");
            }
            if (in_fd != STDIN_FILENO && in_fd > -1) close(in_fd);
            if (out_fd != STDOUT_FILENO && out_fd > -1) close(out_fd);
            free_command_t(&command);
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