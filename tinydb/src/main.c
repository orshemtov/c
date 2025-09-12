#include "tinydb.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static const char *skip_spaces(const char *s)
{
    while (*s == ' ' || *s == '\t')
    {
        s++;
    }
    return s;
}

static bool is_cmd(const char *line, const char *cmd, const char **out_args)
{
    size_t n = strlen(cmd);
    if (strncmp(line, cmd, n) != 0)
        return false;
    const char *p = line + n;
    if (*p != '\0' && *p != ' ' && *p != '\t')
        return false;
    *out_args = skip_spaces(p);
    return true;
}

int main(void)
{
    TinyDb *db = NULL;
    if (tinydb_new("data.tdb", &db) != TDB_OK)
    {
        fprintf(stderr, "Failed to open database\n");
        return 1;
    }

    char line[256];
    printf("TinyDB shell. Type 'exit' to quit.\n");

    for (;;)
    {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin))
        {
            break;
        }

        line[strcspn(line, "\n")] = 0;
        const char *args = NULL;

        if (is_cmd(line, "exit", &args))
        {
            break;
        }
        else if (is_cmd(line, "set", &args))
        {
            args = skip_spaces(args);
            uint32_t key = 0;
            char *endptr = NULL;
            key = (uint32_t)strtoul(args, &endptr, 10);
            if (endptr == args) // No number was parsed
            {
                printf("Usage: set <key> <value>\n");
                continue;
            }

            const char *valstart = skip_spaces(endptr);
            if (*valstart == '\0')
            {
                printf("Usage: set <key> <value>\n");
                continue;
            }

            uint8_t value[VALUE_SIZE];
            memset(value, 0, VALUE_SIZE);
            strncpy((char *)value, valstart, VALUE_SIZE - 1); // Keep last byte zero
            TdbStatus status = tinydb_set(db, key, value);
            if (status == TDB_OK)
            {
                printf("OK\n");
            }
            else
            {
                printf("ERROR\n");
            }
        }
        else if (is_cmd(line, "get", &args))
        {
            args = skip_spaces(args);
            uint32_t key = 0;
            char *endptr = NULL;
            key = (uint32_t)strtoul(args, &endptr, 10);
            if (endptr == args)
            {
                printf("Usage: get <key>\n");
                continue;
            }

            Record record;
            TdbStatus status = tinydb_get(db, key, &record);
            if (status == TDB_OK)
            {
                printf("%s\n", record.value);
            }
            else if (status == TDB_ERR_NOT_FOUND)
            {
                printf("NOT FOUND\n");
            }
            else
            {
                printf("ERROR\n");
            }
        }
        else if (is_cmd(line, "delete", &args))
        {
            args = skip_spaces(args);
            uint32_t key = 0;
            char *endptr = NULL;
            key = (uint32_t)strtoul(args, &endptr, 10);
            if (endptr == args) // No number was parsed
            {
                printf("Usage: delete <key>\n");
                continue;
            }

            TdbStatus status = tinydb_delete(db, key);
            if (status == TDB_OK)
            {
                printf("OK\n");
            }
            else if (status == TDB_ERR_NOT_FOUND)
            {
                printf("NOT FOUND\n");
            }
            else
            {
                printf("ERROR\n");
            }
        }
        else if (*line == '\0')
        {
            continue; // Ignore empty lines
        }
        else
        {
            printf("Unknown command. Available commands: set, get, exit\n");
        }
    }

    return 0;
}
