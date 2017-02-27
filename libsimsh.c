//
// Created by Shea on 2017-02-25.
//

#include <stdio.h>
#include <string.h>
#include "libsimsh.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>

#define typeof __typeof__

void print_prompt()
{
    printf("simsh: ");
}

bool streq(const char * str1, const char * str2)
{
    return strcmp(str1, str2) == 0;
}

validation_t validate_line(const chopped_line_t *line)
{
    bool has_out_redirect = false, has_in_redirect = false;
    for (int i = 0; i < line->num_tokens; ++i)
    {
        char *const cur_token = line->tokens[i];
        if (i < line->num_tokens - 1 && streq(cur_token, "&")) return BAD_AMPERSAND;
        if (streq(cur_token, ">") || streq(cur_token, ">>"))
        {
            if (has_out_redirect) return AMBIGUOUS_OUTPUT_REDIRECT;
            has_out_redirect = true;
            if (i >= line->num_tokens - 1) return MISSING_REDIRECT_NAME;
            char *const next_token = line->tokens[i + 1];
            if (streq(next_token, ">") ||
                streq(next_token, "<") ||
                streq(next_token, ">>"))
                return MISSING_REDIRECT_NAME;
        }
        if (streq(cur_token, "<"))
        {
            if (has_in_redirect) return AMBIGUOUS_INPUT_REDIRECT;
            has_in_redirect = true;
            if (i >= line->num_tokens - 1) return MISSING_REDIRECT_NAME;
            char *const next_token = line->tokens[i + 1];
            if (streq(next_token, ">") ||
                streq(next_token, "<") ||
                streq(next_token, ">>"))
                return MISSING_REDIRECT_NAME;
        }
    }
    int min_tokens = 0;
    if (has_in_redirect) min_tokens += 2;
    if (has_out_redirect) min_tokens += 2;
    if (streq(line->tokens[line->num_tokens - 1], "&")) min_tokens++;
    if (line->num_tokens <= min_tokens) return NULL_COMMAND;
    return VALID;
}

const char* get_validation_result(validation_t result)
{
    switch(result)
    {
        case VALID: return "Valid";
        case BAD_AMPERSAND: return "Out-of-place ampersand";
        case AMBIGUOUS_INPUT_REDIRECT: return "Ambiguous input redirect.";
        case AMBIGUOUS_OUTPUT_REDIRECT: return "Ambiguous output redirect.";
        case MISSING_REDIRECT_NAME: return "Missing name for redirect.";
        case NULL_COMMAND: return "Invalid null command.";
        default: return NULL;
    }
}

command_t parse_chopped_line(chopped_line_t *line)
{
    command_t command = {
            .input_file = NULL,
            .output_file = NULL,
            .args = NULL,
            .foreground = true,
            .create_output = false
    };

    unsigned int argc = line->num_tokens;
    for (int i = 0; i < line->num_tokens; i++)
    {
        char *const cur_token = line->tokens[i];
        if (streq(cur_token, "&"))
        {
            command.foreground = false;
            argc--;
            continue;
        }
        if (streq(cur_token, ">") || streq(cur_token, ">>"))
        {
            argc -= 2;
            command.output_file = strdup(line->tokens[i + 1]);
            command.create_output = streq(line->tokens[i], ">");
            i++;
            continue;
        }
        if (streq(cur_token, "<"))
        {
            argc -= 2;
            command.input_file = strdup(line->tokens[i + 1]);
            i++;
            continue;
        }
    }

    command.args = malloc((argc + 1) * sizeof(char *));
    int args_index = 0;
    for (int j = 0; j < argc; ++j)
    {
        const char *current_token = line->tokens[j];
        if (streq(current_token, "&")) j++;
        if (streq(current_token, ">") ||
            streq(current_token, ">>") ||
            streq(current_token, "<"))
            j += 2;
        command.args[args_index] = strdup(line->tokens[j]);
        args_index++;
    }
    command.args[argc] = NULL;

    return command;
}

void free_command_t(command_t * command)
{
    if (command->input_file != NULL) free(command->input_file);
    if (command->output_file != NULL) free(command->output_file);

    if (command->args != NULL)
    {
        for (int i = 0; command->args[i] != NULL; i++)
            free(command->args[i]);
        free(command->args);
    }
}

bool launch_process(char * const *args, bool wait, int in_fd, int out_fd)
{
    //code modified from: https://stackoverflow.com/questions/15497224/return-status-of-execve
    int pipe_fds[2];
    pipe(pipe_fds);
    pid_t pid;
    if (!(pid = fork())) //child
    {
        close(pipe_fds[0]); //close read end of pipe
        fcntl(pipe_fds[1], F_SETFD, FD_CLOEXEC); //pipe will automatically close on successful exec call

        if (in_fd != STDIN_FILENO) dup2(in_fd, STDIN_FILENO);
        if (out_fd != STDOUT_FILENO) dup2(out_fd, STDOUT_FILENO);
        execvp(args[0], args);
        write(pipe_fds[1], (char *) &errno, sizeof(errno)); //if execvp succeeds, will never reach here
        _exit(EXIT_FAILURE);
    }
    else //parent
    {
        if (pid < 0) return false;
        typeof(errno) pipe_err;
        close(pipe_fds[1]);
        if (read(pipe_fds[0], &pipe_err, sizeof(errno)) == 0)
        {
            if (wait) waitpid(pid, NULL, 0);
            return true;
        }
        else
        {
            errno = pipe_err;
            return false;
        }
    }
    return false;
}


//from: http://www.microhowto.info/howto/reap_zombie_processes_using_a_sigchld_handler.html
void handle_sigchld(int sig) {
    int saved_errno = errno;
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
    errno = saved_errno;
}

void setup_sigchld_handler()
{
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror(0);
        exit(1);
    }
}

#undef typeof