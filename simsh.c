#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <sys/fcntl.h>
#include <unistd.h>
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
            //command_t command = parse_chopped_line(chopped_line);
            command_t * commands;
            int command_count = parse_pipeline(chopped_line, &commands);
            bool error = false;
            int in_fd = STDIN_FILENO;
            int out_fd = STDOUT_FILENO;
            const command_t first_command = commands[0];
            if (first_command.input_file)
            {
                in_fd = open(first_command.input_file, O_RDONLY);
                if (in_fd < 0)
                {
                    error = true;
                    perror(first_command.input_file);
                }
            }
            const command_t final_command = commands[command_count - 1];
            if (!error && final_command.output_file)
            {
                int flags = O_WRONLY;
                if (final_command.create_output) flags |= O_CREAT | O_EXCL;
                else flags |= O_APPEND;
                out_fd = open(final_command.output_file, flags, 0666);
                if (out_fd < 0)
                {
                    error = true;
                    perror(final_command.output_file);
                }
            }
            pid_t command_pids[command_count];
            int pipe_fds[command_count][2];
            if(error)
            {
                perror("Error");
            }
            else
            {
                int temp_fds[2];
                pipe_fds[0][0] = in_fd;
                pipe_fds[command_count-1][1] = out_fd;
                for (int i = 0; i < command_count-1; ++i)
                {
                    if (pipe(temp_fds))
                    {
                        perror("pipe");
                        return EXIT_FAILURE;
                    }
                    pipe_fds[i][1] = temp_fds[1];
                    pipe_fds[i+1][0] = temp_fds[0];
                }
                for(int i = 0; i < command_count; ++i)
                {
                    command_pids[i] = launch_process(commands[i].args, false, pipe_fds[i][0], pipe_fds[i][1]);
                    if (command_pids[i] < 0)
                    {
                        perror("Error");
                        break;
                    }
                }
            }
            if (final_command.foreground)
            {
                for (int i = 0; i < command_count; ++i)
                {
                    waitpid(command_pids[i], NULL, 0);
                }
            }
            for (int i = 0; i < command_count; ++i)
            {
                free_command_t(&commands[i]);
            }
            free(commands);
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