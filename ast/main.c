#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source-file> <function-name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    const char *function_name = argv[2];

    FunctionDef func = {0};
    ast_function_init(&func, function_name, NULL, 0, NULL);

    if (!parse_file_into_function(path, &func))
    {
        fprintf(stderr, "Failed to parse file: %s\n", path);
        return EXIT_FAILURE;
    }

    ast_function_print(&func);
    ast_function_free(&func);

    return EXIT_SUCCESS;
}