#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "aux_files/list.h"
#include "aux_files/chop_line.h"
#include "simsh.h"

void print_prompt()
{
    printf("simsh: ");
}

int main()
{
    char* line = NULL;
    size_t lineLength = 0;
    ssize_t read;
    print_prompt();
    while((read = getline(&line, &lineLength, stdin)) != -1)
    {

        print_prompt();
    }
    free(line);
    if (!feof(stdin))
    {
        perror("getline: ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}