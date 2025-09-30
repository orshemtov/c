#include "db.h"
#include "repl.h"
#include <errors.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    const char* path = argc > 1 ? argv[1] : NULL;
    if (!path)
    {
        fprintf(stderr, "Usage: %s <database-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    MiniDB* db = NULL;
    ErrorCode err = mdb_open(path, &db);
    if (err != ERR_OK)
    {
        fprintf(stderr, "Failed to open database: %d\n", err);
        return EXIT_FAILURE;
    }

    for (;;)
    {
        char* line = readline("minidb> ");
        if (!line) break;
        if (*line) add_history(line);

        Tokens tokens;
        tokenize(line, &tokens);

        Statement stmt;
        ErrorCode perr = parse_statement(&tokens, &stmt);
        if (perr != ERR_OK)
        {
            printf("Parse error: %d\n", perr);
            free(line);
            free_tokens(&tokens);
            free_statement(&stmt);
            continue;
        }

        ErrorCode rerr = execute_statement(db, &stmt);
        if (rerr != ERR_OK)
        {
            printf("Execution error: %d\n", rerr);
            free(line);
            free_tokens(&tokens);
            free_statement(&stmt);
            continue;
        }

        free(line);
        free_tokens(&tokens);
        free_statement(&stmt);
    }

    mdb_close(db);

    return EXIT_SUCCESS;
}