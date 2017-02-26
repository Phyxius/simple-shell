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

        execvp(args[0], args);
        write(pipe_fds[1], (char *) &errno, sizeof(errno)); //if execvp succeeds, will never reach here
        _exit(EXIT_FAILURE);
    }
    else //parent
    {
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