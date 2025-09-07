#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <arg>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("fopen failed");
        exit(1);
    }

    uint32_t lines = 0;
    uint32_t length = 0;
    uint32_t words = 0;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp))
    {
        printf("Line: %s", buffer);
        length += strlen(buffer);
        lines++;

        for (size_t i = 0; i < strlen(buffer); i++)
        {
            if (isspace(buffer[i]))
            {
                words++;
            }
        }
    }

    fclose(fp);

    printf("==========================\n");
    printf("Total lines: %u\n", lines);
    printf("Total length: %u\n", length);
    printf("Total words: %u\n", words);

    return 0;
}