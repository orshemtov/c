#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *first_name = "John";
    const char *last_name = "Doe";

    // concat
    char *name = malloc(strlen(first_name) + strlen(last_name) + 1 + 1);

    strcpy(name, first_name);
    strcat(name, " ");
    strcat(name, last_name);
    name[strlen(name)] = '\0';

    printf("Full name: %s\n", name);

    free(name);

    return 0;
}