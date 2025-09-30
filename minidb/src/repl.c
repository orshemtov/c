/*
 * =============================================================================
 * MiniDB REPL (Read-Eval-Print Loop) Implementation
 * =============================================================================
 *
 * This file implements the SQL parsing and execution engine for MiniDB.
 * It takes text SQL commands and converts them into structured data that
 * can be executed by the database engine.
 *
 * ARCHITECTURE OVERVIEW:
 *
 * 1. TOKENIZATION (tokenize function):
 *    - Breaks SQL text into individual tokens
 *    - Handles punctuation, quoted strings, and whitespace
 *    - Example: "CREATE TABLE users (id INT)" -> ["CREATE", "TABLE", "users", "(", "id", "INT", ")"]
 *
 * 2. PARSING (parse_statement function):
 *    - Analyzes token sequences to understand SQL structure
 *    - Converts tokens into Statement structures
 *    - Validates SQL syntax and reports errors
 *
 * 3. EXECUTION (execute_statement function):
 *    - Takes parsed statements and performs database operations
 *    - Currently a placeholder that prints what would be done
 *    - Will eventually interface with storage engine, catalog, etc.
 *
 * 4. MEMORY MANAGEMENT:
 *    - Proper cleanup of all allocated strings and structures
 *    - Prevents memory leaks in long-running REPL sessions
 *
 * SUPPORTED SQL SUBSET:
 *
 * Data Definition Language (DDL):
 *   CREATE TABLE name (col1 type1, col2 type2, ...)
 *   DROP TABLE name
 *   CREATE INDEX name ON table (column_index)
 *   DROP INDEX name
 *
 * Data Manipulation Language (DML):
 *   INSERT INTO table VALUES (val1, val2, ...)
 *   SELECT * FROM table [WHERE col = value]
 *   UPDATE table SET col = value [WHERE col = value]
 *   DELETE FROM table [WHERE col = value]
 *
 * Meta Commands:
 *   LIST TABLES
 *   HELP
 *   EXIT / QUIT
 *
 * DESIGN DECISIONS:
 *
 * - Column references use indices (0, 1, 2...) instead of names for simplicity
 * - Only equality predicates (=) in WHERE clauses for now
 * - Case-insensitive SQL keywords but case-preserving for identifiers
 * - Memory safety with proper cleanup and error handling
 * - Modular design with separate functions for each statement type
 */

#include "repl.h"
#include "errors.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * =============================================================================
 * HELPER FUNCTIONS FOR TOKEN MANIPULATION
 * =============================================================================
 */

/**
 * Look at the current token without consuming it.
 * Returns NULL if we've reached the end of tokens.
 */
static const char* tokens_peek(const Tokens* tokens)
{
    if (tokens->pos < tokens->count)
    {
        return tokens->items[tokens->pos];
    }
    return NULL;
}

/**
 * Get the current token and advance to the next one.
 * Returns NULL if we've reached the end of tokens.
 */
static const char* tokens_next(Tokens* tokens)
{
    if (tokens->pos < tokens->count)
    {
        return tokens->items[tokens->pos++];
    }
    return NULL;
}

/**
 * Case-insensitive string comparison.
 * Returns 0 if strings are equal (like strcmp), 1 if different.
 * This allows SQL keywords to be matched regardless of case.
 */
static int tokens_ieq(const char* a, const char* b)
{
    if (!a || !b)
    {
        return 1; // Different if either is NULL
    }

    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
        {
            return 1; // Characters differ
        }
        a++;
        b++;
    }

    // Both strings should end at the same time for equality
    return (*a == 0 && *b == 0) ? 0 : 1;
}

/**
 * Add a new token to the tokens array.
 * This dynamically grows the array and makes a copy of the token string.
 */
static void add_token(Tokens* tokens, const char* token)
{
    // Grow the array by one element
    tokens->items = realloc(tokens->items, sizeof(char*) * (tokens->count + 1));

    // Make a copy of the token string (caller doesn't need to manage memory)
    tokens->items[tokens->count] = strdup(token);

    tokens->count++;
}

/*
 * =============================================================================
 * TOKENIZATION FUNCTIONS
 * =============================================================================
 */

/**
 * Break down a SQL command line into individual tokens.
 *
 * This function handles:
 * - Whitespace separation
 * - SQL punctuation (parentheses, commas, equals signs)
 * - Quoted strings (preserving quotes for later parsing)
 * - Buffer management to prevent overflows
 *
 * Example: "CREATE TABLE users (id INT)" becomes:
 * ["CREATE", "TABLE", "users", "(", "id", "INT", ")"]
 */
void tokenize(const char* line, Tokens* out_tokens)
{
    if (!line || !out_tokens)
    {
        return;
    }

    // Initialize the output tokens structure
    out_tokens->items = NULL;
    out_tokens->count = 0;
    out_tokens->pos = 0;

    const char* ptr = line;
    char buffer[256]; // Temporary buffer for building tokens
    int buf_idx = 0;  // Current position in buffer

    while (*ptr)
    {
        // Skip whitespace characters (space, tab, newline, etc.)
        if (isspace(*ptr))
        {
            // If we have characters in buffer, flush them as a token
            if (buf_idx > 0)
            {
                buffer[buf_idx] = '\0';
                add_token(out_tokens, buffer);
                buf_idx = 0;
            }
            ptr++;
            continue;
        }

        // Handle special SQL characters that should be separate tokens
        // These are: ( ) , = ' "
        if (*ptr == '(' || *ptr == ')' || *ptr == ',' ||
            *ptr == '=' || *ptr == '\'' || *ptr == '"')
        {

            // First, flush any current token
            if (buf_idx > 0)
            {
                buffer[buf_idx] = '\0';
                add_token(out_tokens, buffer);
                buf_idx = 0;
            }

            // Handle quoted strings specially - keep quotes with the content
            if (*ptr == '\'' || *ptr == '"')
            {
                char quote_char = *ptr;
                buffer[buf_idx++] = *ptr++; // Include opening quote

                // Read until matching closing quote
                while (*ptr && *ptr != quote_char)
                {
                    buffer[buf_idx++] = *ptr++;
                }

                // Include closing quote if found
                if (*ptr == quote_char)
                {
                    buffer[buf_idx++] = *ptr++;
                }

                buffer[buf_idx] = '\0';
                add_token(out_tokens, buffer);
                buf_idx = 0;
            }
            else
            {
                // Single character punctuation token
                buffer[0] = *ptr;
                buffer[1] = '\0';
                add_token(out_tokens, buffer);
                ptr++;
            }
            continue;
        }

        // Regular character - add to current token buffer
        buffer[buf_idx++] = *ptr++;

        // Prevent buffer overflow by flushing when near capacity
        if (buf_idx >= (int)sizeof(buffer) - 1)
        {
            buffer[buf_idx] = '\0';
            add_token(out_tokens, buffer);
            buf_idx = 0;
        }
    }

    // Don't forget the last token if there's something in the buffer
    if (buf_idx > 0)
    {
        buffer[buf_idx] = '\0';
        add_token(out_tokens, buffer);
    }
}

/**
 * Free all memory allocated for tokens.
 * This includes both the individual token strings and the array that holds them.
 */
void free_tokens(Tokens* tokens)
{
    if (!tokens)
    {
        return;
    }

    // Free each individual token string
    for (int i = 0; i < tokens->count; i++)
    {
        free(tokens->items[i]);
    }

    // Free the array of string pointers
    free(tokens->items);

    // Reset the structure to a clean state
    tokens->items = NULL;
    tokens->count = 0;
    tokens->pos = 0;
}

/*
 * =============================================================================
 * PARSING HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * Parse an optional WHERE clause.
 *
 * Expected format: WHERE <column_index> = <value>
 *
 * Examples:
 *   WHERE 0 = 123        (column 0 equals integer 123)
 *   WHERE 1 = 'John'     (column 1 equals text 'John')
 *
 * Note: We use column indices (0, 1, 2...) instead of column names
 * to simplify the implementation.
 */
static ErrorCode parse_where_clause(Tokens* t, WherePred* where)
{
    // Initialize with no predicate
    where->has_pred = false;

    const char* token = tokens_peek(t);
    if (token && tokens_ieq(token, "WHERE") == 0)
    {
        tokens_next(t); // Consume the WHERE keyword

        // Parse column index (expecting a number like 0, 1, 2...)
        token = tokens_next(t);
        if (!token)
        {
            return ERR_PARSE;
        }

        char* endptr;
        long col_idx = strtol(token, &endptr, 10);
        if (*endptr != '\0' || col_idx < 0)
        {
            return ERR_PARSE; // Not a valid integer or negative
        }
        where->col = (uint16_t)col_idx;

        // Parse operator (currently only = is supported)
        token = tokens_next(t);
        if (!token || strcmp(token, "=") != 0)
        {
            return ERR_PARSE;
        }
        where->op = OP_EQ;

        // Parse the value to compare against
        token = tokens_next(t);
        if (!token)
        {
            return ERR_PARSE;
        }

        // Try to parse as integer first
        char* int_endptr;
        long int_val = strtol(token, &int_endptr, 10);
        if (*int_endptr == '\0')
        {
            // Successfully parsed as integer
            where->value = mdb_value_int(int_val);
        }
        else
        {
            // Parse as text - remove quotes if present
            const char* text_start = token;
            uint16_t text_len = strlen(token);

            // Check if string is quoted and remove quotes
            if (text_len >= 2 && token[0] == '\'' && token[text_len - 1] == '\'')
            {
                text_start = token + 1; // Skip opening quote
                text_len -= 2;          // Remove both quotes from length
            }

            // Make a copy of the text (without quotes)
            char* text_copy = malloc(text_len + 1);
            if (!text_copy)
            {
                return ERR_UNKNOWN;
            }
            strncpy(text_copy, text_start, text_len);
            text_copy[text_len] = '\0';

            where->value = mdb_value_text(text_copy, text_len);
        }

        where->has_pred = true;
    }

    return ERR_OK;
}

/**
 * Parse column definitions for CREATE TABLE statements.
 *
 * Expected format: (column1 type1, column2 type2, ...)
 *
 * Example: (id INT, name TEXT, age INTEGER)
 *
 * Supported types:
 * - INT, INTEGER: for integer columns
 * - TEXT, VARCHAR: for text columns
 */
static ErrorCode parse_column_definitions(Tokens* t, MDBColumnDef* cols, uint16_t* ncols)
{
    *ncols = 0;

    // Must start with opening parenthesis
    const char* token = tokens_next(t);
    if (!token || strcmp(token, "(") != 0)
    {
        return ERR_PARSE;
    }

    // Parse each column definition until we hit the closing parenthesis
    while ((token = tokens_peek(t)) != NULL && strcmp(token, ")") != 0)
    {
        // Prevent too many columns (arbitrary limit of 64)
        if (*ncols >= 64)
        {
            return ERR_PARSE;
        }

        // Get column name
        const char* col_name = tokens_next(t);
        if (!col_name)
        {
            return ERR_PARSE;
        }

        // Make a copy of the column name for storage
        cols[*ncols].name = strdup(col_name);

        // Get column type
        token = tokens_next(t);
        if (!token)
        {
            return ERR_PARSE;
        }

        // Map type names to our internal type enum
        if (tokens_ieq(token, "INT") == 0 || tokens_ieq(token, "INTEGER") == 0)
        {
            cols[*ncols].type = COL_TYPE_INT;
        }
        else if (tokens_ieq(token, "TEXT") == 0 || tokens_ieq(token, "VARCHAR") == 0)
        {
            cols[*ncols].type = COL_TYPE_TEXT;
        }
        else
        {
            return ERR_PARSE; // Unsupported column type
        }

        (*ncols)++;

        // Check if there's a comma (more columns) or closing parenthesis (done)
        token = tokens_peek(t);
        if (token && strcmp(token, ",") == 0)
        {
            tokens_next(t); // Consume the comma
        }
        // If it's not a comma, the while loop will check if it's a ")"
    }

    // Must end with closing parenthesis
    if (!tokens_next(t))
    {
        return ERR_PARSE;
    }

    return ERR_OK;
}

/*
 * =============================================================================
 * MAIN STATEMENT PARSING FUNCTION
 * =============================================================================
 */

/**
 * Parse a complete SQL statement from tokens.
 *
 * This is the main entry point for parsing. It examines the first token
 * to determine the statement type and then delegates to specific parsing
 * logic for each statement type.
 *
 * Supported statements:
 * - CREATE TABLE name (columns...)
 * - DROP TABLE name
 * - CREATE INDEX name ON table (column)
 * - DROP INDEX name
 * - INSERT INTO table VALUES (values...)
 * - SELECT * FROM table [WHERE condition]
 * - UPDATE table SET assignments... [WHERE condition]
 * - DELETE FROM table [WHERE condition]
 * - LIST TABLES
 * - HELP
 * - EXIT/QUIT
 */
ErrorCode parse_statement(const Tokens* tokens, Statement* out_stmt)
{
    // Make a local copy of tokens so we can advance through them
    // without modifying the caller's copy
    Tokens t = *tokens;

    // Look at the first token to determine statement type
    const char* first = tokens_peek(&t);
    if (!first)
    {
        return ERR_PARSE; // Empty statement
    }

    // Handle CREATE statements (TABLE or INDEX)
    if (tokens_ieq(first, "CREATE") == 0)
    {
        tokens_next(&t); // Consume "CREATE"

        const char* second = tokens_next(&t);
        if (!second)
        {
            return ERR_PARSE;
        }

        if (tokens_ieq(second, "TABLE") == 0)
        {
            // CREATE TABLE name (column definitions...)
            const char* name = tokens_next(&t);
            if (!name)
            {
                return ERR_PARSE;
            }

            out_stmt->kind = STMT_CREATE_TABLE;
            out_stmt->create_table.name = strdup(name);

            // Parse the column definitions: (col1 type1, col2 type2, ...)
            return parse_column_definitions(&t, out_stmt->create_table.cols,
                                            &out_stmt->create_table.ncols);
        }
        else if (tokens_ieq(second, "INDEX") == 0)
        {
            // CREATE INDEX name ON table (column_index)
            const char* name = tokens_next(&t);
            if (!name)
            {
                return ERR_PARSE;
            }

            // Expect "ON" keyword
            const char* on = tokens_next(&t);
            if (!on || tokens_ieq(on, "ON") != 0)
            {
                return ERR_PARSE;
            }

            // Get the table name
            const char* table_name = tokens_next(&t);
            if (!table_name)
            {
                return ERR_PARSE;
            }

            // Expect opening parenthesis for column specification
            const char* paren = tokens_next(&t);
            if (!paren || strcmp(paren, "(") != 0)
            {
                return ERR_PARSE;
            }

            // Get the column index (we use column numbers like 0, 1, 2...)
            const char* col_idx_str = tokens_next(&t);
            if (!col_idx_str)
            {
                return ERR_PARSE;
            }

            // Parse the column index as an integer
            char* endptr;
            long col_idx = strtol(col_idx_str, &endptr, 10);
            if (*endptr != '\0' || col_idx < 0)
            {
                return ERR_PARSE;
            }

            // Expect closing parenthesis
            paren = tokens_next(&t);
            if (!paren || strcmp(paren, ")") != 0)
            {
                return ERR_PARSE;
            }

            out_stmt->kind = STMT_CREATE_INDEX;
            out_stmt->create_index.name = strdup(name);
            out_stmt->create_index.table_name = strdup(table_name);
            out_stmt->create_index.col_idx = (uint16_t)col_idx;
            out_stmt->create_index.is_unique = false; // Default to non-unique

            return ERR_OK;
        }
        else
        {
            return ERR_PARSE; // Unknown CREATE statement type
        }
    }

    // Handle DROP statements (TABLE or INDEX)
    else if (tokens_ieq(first, "DROP") == 0)
    {
        tokens_next(&t); // Consume "DROP"

        const char* second = tokens_next(&t);
        if (!second)
        {
            return ERR_PARSE;
        }

        if (tokens_ieq(second, "TABLE") == 0)
        {
            // DROP TABLE name
            const char* name = tokens_next(&t);
            if (!name)
            {
                return ERR_PARSE;
            }

            out_stmt->kind = STMT_DROP_TABLE;
            out_stmt->drop_table.name = strdup(name);
            return ERR_OK;
        }
        else if (tokens_ieq(second, "INDEX") == 0)
        {
            // DROP INDEX name
            const char* name = tokens_next(&t);
            if (!name)
            {
                return ERR_PARSE;
            }

            out_stmt->kind = STMT_DROP_INDEX;
            out_stmt->drop_index.name = strdup(name);
            return ERR_OK;
        }
        else
        {
            return ERR_PARSE; // Unknown DROP statement type
        }
    }

    // Handle INSERT statements
    else if (tokens_ieq(first, "INSERT") == 0)
    {
        tokens_next(&t); // Consume "INSERT"

        // Expect "INTO" keyword
        const char* into = tokens_next(&t);
        if (!into || tokens_ieq(into, "INTO") != 0)
        {
            return ERR_PARSE;
        }

        // Get table name
        const char* table_name = tokens_next(&t);
        if (!table_name)
        {
            return ERR_PARSE;
        }

        const char* values_kw = tokens_next(&t);
        if (!values_kw || tokens_ieq(values_kw, "VALUES") != 0) return ERR_PARSE;

        // Expect opening parenthesis
        const char* paren = tokens_next(&t);
        if (!paren || strcmp(paren, "(") != 0) return ERR_PARSE;

        out_stmt->kind = STMT_INSERT;
        out_stmt->insert_.table_name = strdup(table_name);
        out_stmt->insert_.nvalues = 0;

        const char* token;
        while ((token = tokens_peek(&t)) != NULL && strcmp(token, ")") != 0)
        {
            if (out_stmt->insert_.nvalues >= 128) return ERR_PARSE; // Too many values

            token = tokens_next(&t);

            // Try to parse as integer first
            char* endptr;
            long int_val = strtol(token, &endptr, 10);
            if (*endptr == '\0')
            {
                // It's an integer
                out_stmt->insert_.values[out_stmt->insert_.nvalues] = mdb_value_int(int_val);
            }
            else
            {
                // Treat as text (remove quotes if present)
                const char* text_start = token;
                uint16_t text_len = strlen(token);

                if (text_len >= 2 && token[0] == '\'' && token[text_len - 1] == '\'')
                {
                    // Remove quotes
                    text_start = token + 1;
                    text_len -= 2;
                }

                char* text_copy = malloc(text_len + 1);
                if (!text_copy) return ERR_UNKNOWN;
                strncpy(text_copy, text_start, text_len);
                text_copy[text_len] = '\0';

                out_stmt->insert_.values[out_stmt->insert_.nvalues] = mdb_value_text(text_copy, text_len);
            }

            out_stmt->insert_.nvalues++;

            // Check for comma
            token = tokens_peek(&t);
            if (token && strcmp(token, ",") == 0)
            {
                tokens_next(&t); // consume comma
            }
        }

        // Consume closing parenthesis
        if (!tokens_next(&t)) return ERR_PARSE;

        return ERR_OK;
    }
    else if (tokens_ieq(first, "SELECT") == 0)
    {
        tokens_next(&t);
        const char* star = tokens_next(&t);
        if (!star || strcmp(star, "*") != 0) return ERR_PARSE;

        const char* from = tokens_next(&t);
        if (!from || tokens_ieq(from, "FROM") != 0) return ERR_PARSE;

        const char* table_name = tokens_next(&t);
        if (!table_name) return ERR_PARSE;

        out_stmt->kind = STMT_SELECT;
        out_stmt->select_.table_name = strdup(table_name);

        return parse_where_clause(&t, &out_stmt->select_.where);
    }
    else if (tokens_ieq(first, "DELETE") == 0)
    {
        tokens_next(&t);
        const char* from = tokens_next(&t);
        if (!from || tokens_ieq(from, "FROM") != 0) return ERR_PARSE;

        const char* table_name = tokens_next(&t);
        if (!table_name) return ERR_PARSE;

        out_stmt->kind = STMT_DELETE;
        out_stmt->delete_.table_name = strdup(table_name);

        return parse_where_clause(&t, &out_stmt->delete_.where);
    }
    else if (tokens_ieq(first, "UPDATE") == 0)
    {
        tokens_next(&t);
        const char* table_name = tokens_next(&t);
        if (!table_name) return ERR_PARSE;

        const char* set = tokens_next(&t);
        if (!set || tokens_ieq(set, "SET") != 0) return ERR_PARSE;

        out_stmt->kind = STMT_UPDATE;
        out_stmt->update_.table_name = strdup(table_name);
        out_stmt->update_.nvalues = 0;

        // Parse column=value pairs
        const char* token;
        while ((token = tokens_peek(&t)) != NULL && tokens_ieq(token, "WHERE") != 0)
        {
            if (out_stmt->update_.nvalues >= 128) return ERR_PARSE; // Too many values

            // Skip column name (we assume positional updates)
            tokens_next(&t);

            // Expect = sign
            const char* eq = tokens_next(&t);
            if (!eq || strcmp(eq, "=") != 0) return ERR_PARSE;

            // Parse value
            const char* value_token = tokens_next(&t);
            if (!value_token) return ERR_PARSE;

            // Try to parse as integer first
            char* endptr;
            long int_val = strtol(value_token, &endptr, 10);
            if (*endptr == '\0')
            {
                // It's an integer
                out_stmt->update_.values[out_stmt->update_.nvalues] = mdb_value_int(int_val);
            }
            else
            {
                // Treat as text (remove quotes if present)
                const char* text_start = value_token;
                uint16_t text_len = strlen(value_token);

                if (text_len >= 2 && value_token[0] == '\'' && value_token[text_len - 1] == '\'')
                {
                    // Remove quotes
                    text_start = value_token + 1;
                    text_len -= 2;
                }

                char* text_copy = malloc(text_len + 1);
                if (!text_copy) return ERR_UNKNOWN;
                strncpy(text_copy, text_start, text_len);
                text_copy[text_len] = '\0';

                out_stmt->update_.values[out_stmt->update_.nvalues] = mdb_value_text(text_copy, text_len);
            }

            out_stmt->update_.nvalues++;

            // Check for comma
            token = tokens_peek(&t);
            if (token && strcmp(token, ",") == 0)
            {
                tokens_next(&t); // consume comma
            }
        }

        return parse_where_clause(&t, &out_stmt->update_.where);
    }
    else if (tokens_ieq(first, "LIST") == 0)
    {
        tokens_next(&t);
        const char* second = tokens_next(&t);
        if (!second || tokens_ieq(second, "TABLES") != 0) return ERR_PARSE;

        out_stmt->kind = STMT_LIST_TABLES;
        return ERR_OK;
    }
    else if (tokens_ieq(first, "HELP") == 0)
    {
        out_stmt->kind = STMT_HELP;
        return ERR_OK;
    }
    else if (tokens_ieq(first, "EXIT") == 0 || tokens_ieq(first, "QUIT") == 0)
    {
        out_stmt->kind = STMT_EXIT;
        return ERR_OK;
    }
    else
    {
        return ERR_UNSUPPORTED;
    }
}

/*
 * =============================================================================
 * MEMORY MANAGEMENT FUNCTIONS
 * =============================================================================
 */

/**
 * Free all dynamically allocated memory in a Statement.
 *
 * This function is crucial for preventing memory leaks. Since our parser
 * creates copies of strings using strdup(), we need to free them properly.
 * Different statement types allocate different resources:
 *
 * - CREATE TABLE: table name + all column names
 * - INSERT/UPDATE: table name + text values in the data
 * - WHERE clauses: text values in predicates
 * - etc.
 */
void free_statement(Statement* stmt)
{
    if (!stmt)
    {
        return;
    }

    switch (stmt->kind)
    {
    case STMT_CREATE_TABLE:
        free((char*)stmt->create_table.name);
        for (int i = 0; i < stmt->create_table.ncols; i++)
        {
            free((char*)stmt->create_table.cols[i].name);
        }
        break;
    case STMT_DROP_TABLE:
        free((char*)stmt->drop_table.name);
        break;
    case STMT_CREATE_INDEX:
        free((char*)stmt->create_index.name);
        free((char*)stmt->create_index.table_name);
        break;
    case STMT_DROP_INDEX:
        free((char*)stmt->drop_index.name);
        break;
    case STMT_INSERT:
        free((char*)stmt->insert_.table_name);
        for (int i = 0; i < stmt->insert_.nvalues; i++)
        {
            if (!stmt->insert_.values[i].is_null &&
                stmt->insert_.values[i].type == COL_TYPE_TEXT)
            {
                free((char*)stmt->insert_.values[i].text.ptr);
            }
        }
        break;
    case STMT_SELECT:
        free((char*)stmt->select_.table_name);
        if (stmt->select_.where.has_pred &&
            !stmt->select_.where.value.is_null &&
            stmt->select_.where.value.type == COL_TYPE_TEXT)
        {
            free((char*)stmt->select_.where.value.text.ptr);
        }
        break;
    case STMT_DELETE:
        free((char*)stmt->delete_.table_name);
        if (stmt->delete_.where.has_pred &&
            !stmt->delete_.where.value.is_null &&
            stmt->delete_.where.value.type == COL_TYPE_TEXT)
        {
            free((char*)stmt->delete_.where.value.text.ptr);
        }
        break;
    case STMT_UPDATE:
        free((char*)stmt->update_.table_name);
        for (int i = 0; i < stmt->update_.nvalues; i++)
        {
            if (!stmt->update_.values[i].is_null &&
                stmt->update_.values[i].type == COL_TYPE_TEXT)
            {
                free((char*)stmt->update_.values[i].text.ptr);
            }
        }
        if (stmt->update_.where.has_pred &&
            !stmt->update_.where.value.is_null &&
            stmt->update_.where.value.type == COL_TYPE_TEXT)
        {
            free((char*)stmt->update_.where.value.text.ptr);
        }
        break;
    case STMT_LIST_TABLES:
    case STMT_HELP:
    case STMT_EXIT:
        // No resources to free
        break;
    default:
        break;
    }
}

/*
 * =============================================================================
 * STATEMENT EXECUTION FUNCTIONS
 * =============================================================================
 */

/**
 * Execute a parsed SQL statement.
 *
 * Currently this is a placeholder implementation that just prints what
 * would be done. In a complete database implementation, this function
 * would:
 *
 * - Create/drop tables in the catalog
 * - Insert/update/delete rows in table files
 * - Create/drop indexes
 * - Query data and return results
 *
 * For now, it serves as a demonstration of the parsing results and
 * provides a framework for future implementation.
 */
ErrorCode execute_statement(MiniDB* db, const Statement* stmt)
{
    if (!stmt)
    {
        return ERR_INVALID;
    }

    // Database parameter is unused in placeholder implementation
    (void)db;

    switch (stmt->kind)
    {
    case STMT_LIST_TABLES:
        printf("Listing tables...\n");
        // TODO: Implement actual table listing
        break;

    case STMT_CREATE_TABLE:
        printf("Creating table '%s' with %d columns\n",
               stmt->create_table.name, stmt->create_table.ncols);
        // TODO: Implement actual table creation
        break;

    case STMT_DROP_TABLE:
        printf("Dropping table '%s'\n", stmt->drop_table.name);
        // TODO: Implement actual table dropping
        break;

    case STMT_CREATE_INDEX:
        printf("Creating index '%s' on table '%s' column %d\n",
               stmt->create_index.name, stmt->create_index.table_name,
               stmt->create_index.col_idx);
        // TODO: Implement actual index creation
        break;

    case STMT_DROP_INDEX:
        printf("Dropping index '%s'\n", stmt->drop_index.name);
        // TODO: Implement actual index dropping
        break;

    case STMT_INSERT:
        printf("Inserting %d values into table '%s'\n",
               stmt->insert_.nvalues, stmt->insert_.table_name);
        // TODO: Implement actual row insertion
        break;

    case STMT_SELECT:
        printf("Selecting from table '%s'", stmt->select_.table_name);
        if (stmt->select_.where.has_pred)
        {
            printf(" with WHERE clause on column %d", stmt->select_.where.col);
        }
        printf("\n");
        // TODO: Implement actual row selection
        break;

    case STMT_DELETE:
        printf("Deleting from table '%s'", stmt->delete_.table_name);
        if (stmt->delete_.where.has_pred)
        {
            printf(" with WHERE clause on column %d", stmt->delete_.where.col);
        }
        printf("\n");
        // TODO: Implement actual row deletion
        break;

    case STMT_UPDATE:
        printf("Updating table '%s' with %d values",
               stmt->update_.table_name, stmt->update_.nvalues);
        if (stmt->update_.where.has_pred)
        {
            printf(" with WHERE clause on column %d", stmt->update_.where.col);
        }
        printf("\n");
        // TODO: Implement actual row updating
        break;

    case STMT_HELP:
        printf("Available commands:\n");
        printf("  CREATE TABLE name (col1 type1, col2 type2, ...)\n");
        printf("  DROP TABLE name\n");
        printf("  CREATE INDEX name ON table (column_index)\n");
        printf("  DROP INDEX name\n");
        printf("  INSERT INTO table VALUES (val1, val2, ...)\n");
        printf("  SELECT * FROM table [WHERE col = value]\n");
        printf("  UPDATE table SET col1 = val1 [WHERE col = value]\n");
        printf("  DELETE FROM table [WHERE col = value]\n");
        printf("  LIST TABLES\n");
        printf("  HELP\n");
        printf("  EXIT\n");
        break;

    case STMT_EXIT:
        printf("Goodbye!\n");
        return ERR_UNSUPPORTED; // Signal to exit the REPL

    default:
        return ERR_UNSUPPORTED;
    }

    return ERR_OK;
}
