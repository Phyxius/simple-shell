#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main()
{
    char* line = NULL;
    size_t lineLength = 0;
    ssize_t read;
    while((read = getline(&line, &lineLength, stdin)) != -1)
    {

    }
    free(line);
    perror("exit: ");
    return 0;
}