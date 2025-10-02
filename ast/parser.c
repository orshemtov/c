#include "ast.h"
#include <stdio.h>

int parse_file_into_function(const char *filename, FunctionDef *func)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return 0;
    }

    char line[4096];
    size_t lineno = 0;

    while (fgets(line, sizeof(line), file))
    {
        lineno++;
        Stmt *stmt = parse_line(line);
        if (!stmt)
            continue;
        ast_function_push_stmt(func, stmt);
    }

    fclose(file);

    return 1;
}

// could be expr, assign or return
Stmt *parse_line(const char *line)
{
}