#include <stdio.h>
#include <stdlib.h>
#include "libsimsh.h"


int main()
{
    char* line = NULL;
    size_t lineLength = 0;
    ssize_t read;
    print_prompt();
    while((read = getline(&line, &lineLength, stdin)) != -1)
    {
        if (line[read - 1] == '\n') line[read -1] = '\0';
        chopped_line_t * chopped_line = get_chopped_line(line);
        free_chopped_line(chopped_line);
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