#include "errors.h"
#include "repl.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_parse_create_table_simple(void)
{
    const char* sql = "CREATE TABLE users (id INT, name TEXT)";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_CREATE_TABLE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.create_table.name);
    TEST_ASSERT_EQUAL(2, stmt.create_table.ncols);

    TEST_ASSERT_EQUAL_STRING("id", stmt.create_table.cols[0].name);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.create_table.cols[0].type);

    TEST_ASSERT_EQUAL_STRING("name", stmt.create_table.cols[1].name);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.create_table.cols[1].type);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_create_table_multiple_columns(void)
{
    const char* sql = "CREATE TABLE products (id INTEGER, name VARCHAR, price INT, description TEXT)";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_CREATE_TABLE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("products", stmt.create_table.name);
    TEST_ASSERT_EQUAL(4, stmt.create_table.ncols);

    TEST_ASSERT_EQUAL_STRING("id", stmt.create_table.cols[0].name);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.create_table.cols[0].type);

    TEST_ASSERT_EQUAL_STRING("name", stmt.create_table.cols[1].name);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.create_table.cols[1].type);

    TEST_ASSERT_EQUAL_STRING("price", stmt.create_table.cols[2].name);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.create_table.cols[2].type);

    TEST_ASSERT_EQUAL_STRING("description", stmt.create_table.cols[3].name);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.create_table.cols[3].type);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_drop_table(void)
{
    const char* sql = "DROP TABLE users";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_DROP_TABLE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.drop_table.name);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_create_index(void)
{
    const char* sql = "CREATE INDEX idx_name ON users (1)";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_CREATE_INDEX, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("idx_name", stmt.create_index.name);
    TEST_ASSERT_EQUAL_STRING("users", stmt.create_index.table_name);
    TEST_ASSERT_EQUAL(1, stmt.create_index.col_idx);
    TEST_ASSERT_EQUAL(false, stmt.create_index.is_unique);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_drop_index(void)
{
    const char* sql = "DROP INDEX idx_name";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_DROP_INDEX, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("idx_name", stmt.drop_index.name);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_insert_integers(void)
{
    const char* sql = "INSERT INTO users VALUES (1, 25, 100)";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_INSERT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.insert_.table_name);
    TEST_ASSERT_EQUAL(3, stmt.insert_.nvalues);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[0].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.insert_.values[0].type);
    TEST_ASSERT_EQUAL(1, stmt.insert_.values[0].integer);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[1].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.insert_.values[1].type);
    TEST_ASSERT_EQUAL(25, stmt.insert_.values[1].integer);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[2].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.insert_.values[2].type);
    TEST_ASSERT_EQUAL(100, stmt.insert_.values[2].integer);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_insert_mixed_types(void)
{
    const char* sql = "INSERT INTO users VALUES (1, 'John Doe', 25)";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_INSERT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.insert_.table_name);
    TEST_ASSERT_EQUAL(3, stmt.insert_.nvalues);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[0].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.insert_.values[0].type);
    TEST_ASSERT_EQUAL(1, stmt.insert_.values[0].integer);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[1].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.insert_.values[1].type);
    TEST_ASSERT_EQUAL_STRING("John Doe", stmt.insert_.values[1].text.ptr);
    TEST_ASSERT_EQUAL(8, stmt.insert_.values[1].text.length);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[2].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.insert_.values[2].type);
    TEST_ASSERT_EQUAL(25, stmt.insert_.values[2].integer);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_insert_text_only(void)
{
    const char* sql = "INSERT INTO messages VALUES ('Hello', 'World', 'Test')";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_INSERT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("messages", stmt.insert_.table_name);
    TEST_ASSERT_EQUAL(3, stmt.insert_.nvalues);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[0].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.insert_.values[0].type);
    TEST_ASSERT_EQUAL_STRING("Hello", stmt.insert_.values[0].text.ptr);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[1].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.insert_.values[1].type);
    TEST_ASSERT_EQUAL_STRING("World", stmt.insert_.values[1].text.ptr);

    TEST_ASSERT_EQUAL(false, stmt.insert_.values[2].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.insert_.values[2].type);
    TEST_ASSERT_EQUAL_STRING("Test", stmt.insert_.values[2].text.ptr);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_select_simple(void)
{
    const char* sql = "SELECT * FROM users";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_SELECT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.select_.table_name);
    TEST_ASSERT_EQUAL(false, stmt.select_.where.has_pred);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_select_with_where_int(void)
{
    const char* sql = "SELECT * FROM users WHERE 0 = 123";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_SELECT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.select_.table_name);
    TEST_ASSERT_EQUAL(true, stmt.select_.where.has_pred);
    TEST_ASSERT_EQUAL(0, stmt.select_.where.col);
    TEST_ASSERT_EQUAL(OP_EQ, stmt.select_.where.op);
    TEST_ASSERT_EQUAL(false, stmt.select_.where.value.is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.select_.where.value.type);
    TEST_ASSERT_EQUAL(123, stmt.select_.where.value.integer);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_select_with_where_text(void)
{
    const char* sql = "SELECT * FROM users WHERE 1 = 'John'";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_SELECT, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.select_.table_name);
    TEST_ASSERT_EQUAL(true, stmt.select_.where.has_pred);
    TEST_ASSERT_EQUAL(1, stmt.select_.where.col);
    TEST_ASSERT_EQUAL(OP_EQ, stmt.select_.where.op);
    TEST_ASSERT_EQUAL(false, stmt.select_.where.value.is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.select_.where.value.type);
    TEST_ASSERT_EQUAL_STRING("John", stmt.select_.where.value.text.ptr);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_update_simple(void)
{
    const char* sql = "UPDATE users SET name = 'Jane'";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_UPDATE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.update_.table_name);
    TEST_ASSERT_EQUAL(1, stmt.update_.nvalues);
    TEST_ASSERT_EQUAL(false, stmt.update_.where.has_pred);

    TEST_ASSERT_EQUAL(false, stmt.update_.values[0].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.update_.values[0].type);
    TEST_ASSERT_EQUAL_STRING("Jane", stmt.update_.values[0].text.ptr);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_update_with_where(void)
{
    const char* sql = "UPDATE users SET age = 30, name = 'Alice' WHERE 0 = 1";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_UPDATE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.update_.table_name);
    TEST_ASSERT_EQUAL(2, stmt.update_.nvalues);
    TEST_ASSERT_EQUAL(true, stmt.update_.where.has_pred);

    // First value (age = 30)
    TEST_ASSERT_EQUAL(false, stmt.update_.values[0].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.update_.values[0].type);
    TEST_ASSERT_EQUAL(30, stmt.update_.values[0].integer);

    // Second value (name = 'Alice')
    TEST_ASSERT_EQUAL(false, stmt.update_.values[1].is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.update_.values[1].type);
    TEST_ASSERT_EQUAL_STRING("Alice", stmt.update_.values[1].text.ptr);

    // WHERE clause
    TEST_ASSERT_EQUAL(0, stmt.update_.where.col);
    TEST_ASSERT_EQUAL(OP_EQ, stmt.update_.where.op);
    TEST_ASSERT_EQUAL(false, stmt.update_.where.value.is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_INT, stmt.update_.where.value.type);
    TEST_ASSERT_EQUAL(1, stmt.update_.where.value.integer);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_delete_simple(void)
{
    const char* sql = "DELETE FROM users";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_DELETE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.delete_.table_name);
    TEST_ASSERT_EQUAL(false, stmt.delete_.where.has_pred);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_delete_with_where(void)
{
    const char* sql = "DELETE FROM users WHERE 2 = 'inactive'";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_DELETE, stmt.kind);
    TEST_ASSERT_EQUAL_STRING("users", stmt.delete_.table_name);
    TEST_ASSERT_EQUAL(true, stmt.delete_.where.has_pred);
    TEST_ASSERT_EQUAL(2, stmt.delete_.where.col);
    TEST_ASSERT_EQUAL(OP_EQ, stmt.delete_.where.op);
    TEST_ASSERT_EQUAL(false, stmt.delete_.where.value.is_null);
    TEST_ASSERT_EQUAL(COL_TYPE_TEXT, stmt.delete_.where.value.type);
    TEST_ASSERT_EQUAL_STRING("inactive", stmt.delete_.where.value.text.ptr);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_list_tables(void)
{
    const char* sql = "LIST TABLES";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_LIST_TABLES, stmt.kind);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_help(void)
{
    const char* sql = "HELP";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_HELP, stmt.kind);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_exit(void)
{
    const char* sql = "EXIT";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_EXIT, stmt.kind);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_quit(void)
{
    const char* sql = "QUIT";

    Tokens tokens;
    tokenize(sql, &tokens);

    Statement stmt;
    ErrorCode err = parse_statement(&tokens, &stmt);

    TEST_ASSERT_EQUAL(ERR_OK, err);
    TEST_ASSERT_EQUAL(STMT_EXIT, stmt.kind);

    free_statement(&stmt);
    free_tokens(&tokens);
}

void test_parse_case_insensitive(void)
{
    const char* commands[] = {
        "select * from users",
        "SELECT * FROM USERS",
        "Select * From Users",
        "insert into users values (1)",
        "INSERT INTO USERS VALUES (1)",
        "Insert Into Users Values (1)"};

    for (int i = 0; i < 6; i++)
    {
        Tokens tokens;
        tokenize(commands[i], &tokens);

        Statement stmt;
        ErrorCode err = parse_statement(&tokens, &stmt);

        TEST_ASSERT_EQUAL_MESSAGE(ERR_OK, err, commands[i]);

        if (i < 3)
        {
            TEST_ASSERT_EQUAL_MESSAGE(STMT_SELECT, stmt.kind, commands[i]);
        }
        else
        {
            TEST_ASSERT_EQUAL_MESSAGE(STMT_INSERT, stmt.kind, commands[i]);
        }

        free_statement(&stmt);
        free_tokens(&tokens);
    }
}

void test_parse_invalid_statements(void)
{
    const char* invalid_commands[] = {
        "",                   // Empty
        "INVALID COMMAND",    // Unknown command
        "CREATE",             // Incomplete CREATE
        "CREATE TABLE",       // Missing table name
        "CREATE TABLE users", // Missing column definitions
        "INSERT INTO",        // Incomplete INSERT
        "SELECT",             // Incomplete SELECT
        "SELECT *",           // Missing FROM
        "SELECT * FROM",      // Missing table name
        "UPDATE",             // Incomplete UPDATE
        "DELETE",             // Incomplete DELETE
        "DROP",               // Incomplete DROP
    };

    int num_invalid = sizeof(invalid_commands) / sizeof(invalid_commands[0]);

    for (int i = 0; i < num_invalid; i++)
    {
        Tokens tokens;
        tokenize(invalid_commands[i], &tokens);

        Statement stmt;
        ErrorCode err = parse_statement(&tokens, &stmt);

        TEST_ASSERT_NOT_EQUAL_MESSAGE(ERR_OK, err, invalid_commands[i]);

        free_tokens(&tokens);
        // Don't free statement on parse error
    }
}