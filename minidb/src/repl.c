#include "repl.h"
#include "errors.h"
#include "string.h"

void tokenize(const char* line, Tokens* out_tokens)
{
    if (!line || !out_tokens)
    {
        return;
    }

    out_tokens->items = NULL;
    out_tokens->count = 0;
    out_tokens->pos = 0;

    char* buf = strdup(line);
    if (!buf)
    {
        return;
    }

    char* saveptr;
    char* token = strtok_r(buf, " \t\r\n", &saveptr);
    while (token)
    {
        out_tokens->items = realloc(out_tokens->items, sizeof(char*) * (out_tokens->count + 1));
        out_tokens->items[out_tokens->count] = strdup(token);
        out_tokens->count++;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    free(buf);
}

void free_tokens(Tokens* tokens)
{
    for (int i = 0; i < tokens->count; i++)
    {
        free(tokens->items[i]);
    }
    free(tokens->items);
    tokens->items = NULL;
    tokens->count = 0;
    tokens->pos = 0;
}

ErrorCode parse_statement(const Tokens* tokens, Statement* out_stmt)
{
    // We need to advance 'pos' as we parse tokens, so we make a copy of the tokens struct
    // to avoid modifying the original one.
    Tokens t = *tokens;

    const char* first = tokens_peek(&t);
    if (!first) return ERR_PARSE;

    if (tokens_ieq(first, "CREATE") == 0)
    {
        tokens_next(&t);
        const char* second = tokens_peek(&t);
        if (!second) return ERR_PARSE;

        if (tokens_ieq(second, "TABLE") == 0)
        {
            const char* name = tokens_next(&t);
            if (!name) return ERR_PARSE;

            out_stmt->kind = STMT_CREATE_TABLE;
            out_stmt->create_table.name = strdup(name);
            out_stmt->create_table.ncols = 0;
        }
        else if (tokens_ieq(second, "INDEX") == 0)
        {
        }
        else
        {
            return ERR_PARSE;
        }
    }
    else if (tokens_ieq(first, "DROP") == 0)
    {
    }
    else if (tokens_ieq(first, "INSERT") == 0)
    {
    }
    else if (tokens_ieq(first, "SELECT") == 0)
    {
    }
    else if (tokens_ieq(first, "DELETE") == 0)
    {
    }
    else if (tokens_ieq(first, "UPDATE") == 0)
    {
    }
    else
    {
        return ERR_UNSUPPORTED;
    }
}

void free_statement(Statement* stmt)
{
    switch (stmt->kind)
    {
    case STMT_CREATE_TABLE:
        // Free resources specific to create_table
        break;
    case STMT_DROP_TABLE:
        // Free resources specific to drop_table
        break;
    case STMT_CREATE_INDEX:
        // Free resources specific to create_index
        break;
    case STMT_DROP_INDEX:
        // Free resources specific to drop_index
        break;
    case STMT_INSERT:
        // Free resources specific to insert_
        break;
    case STMT_SELECT:
        // Free resources specific to select_
        break;
    case STMT_DELETE:
        // Free resources specific to delete_
        break;
    case STMT_UPDATE:
        // Free resources specific to update_
        break;
    default:
        // Handle other statement types if necessary
        break;
    }
}

ErrorCode execute_statement(MiniDB* db, const Statement* stmt)
{
    switch (stmt->kind)
    {
    case STMT_CREATE_TABLE:
        printf("STMT_CREATE_TABLE");
    case STMT_DROP_TABLE:
        printf("STMT_DROP_TABLE");
    case STMT_CREATE_INDEX:
        printf("STMT_CREATE_INDEX");
    case STMT_DROP_INDEX:
        printf("STMT_DROP_INDEX");
    case STMT_INSERT:
        printf("STMT_INSERT");
    case STMT_SELECT:
        printf("STMT_SELECT");
    case STMT_DELETE:
        printf("STMT_DELETE");
    case STMT_UPDATE:
        printf("STMT_UPDATE");
    default:
        return ERR_UNSUPPORTED;
    }
}
