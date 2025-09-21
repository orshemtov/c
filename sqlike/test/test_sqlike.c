#include "unity.h"
#include "sqlike.h"
#include <stdio.h>
#include <stdlib.h>

static sqlk_db_t *db = NULL;
static const char *TEST_DB_PATH = "test.db";

void setUp(void)
{
    remove(TEST_DB_PATH);
    db = NULL;
}

void tearDown(void)
{
    if (db)
    {
        sqlk_close(db);
        db = NULL;
    }
}

void test_open(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
}

void test_alloc_page(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    uint32_t pgno;
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_alloc_page(db, &pgno));
    TEST_ASSERT_EQUAL(1, pgno); // First allocated page should be page 1
}

void test_read_page(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    sqlk_page_t page;
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_read_page(db, 0, &page)); // Read page 0 (header page)
}

void test_write_page(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    // First allocate a page to write to
    uint32_t pgno;
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_alloc_page(db, &pgno));

    sqlk_page_t page;
    // Initialize page with some data
    for (int i = 0; i < SQLK_PAGE_SIZE; i++)
    {
        page.data[i] = (char)i;
    }
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_write_page(db, pgno, &page));
}

void test_create_table_success(void)
{
    // Open DB first
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    // Create a table
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "mytable"));
}

void test_create_table_null_db(void)
{
    // Should fail if db is NULL
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_create_table(NULL, "mytable"));
}

void test_create_table_null_name(void)
{
    // Open DB first
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    // Should fail if name is NULL
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_create_table(db, NULL));
}

void test_insert_row_success(void)
{
    // Open DB and create a table first
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "users"));

    // Insert a row
    sqlk_row_t row = {1, "John Doe"};
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row));
}

void test_insert_row_null_db(void)
{
    sqlk_row_t row = {1, "John Doe"};
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_insert_row(NULL, "users", &row));
}

void test_insert_row_null_table(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    sqlk_row_t row = {1, "John Doe"};
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_insert_row(db, NULL, &row));
}

void test_insert_row_null_row(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "users"));

    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_insert_row(db, "users", NULL));
}

void test_insert_row_table_not_found(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);

    sqlk_row_t row = {1, "John Doe"};
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_insert_row(db, "nonexistent", &row));
}

void test_insert_multiple_rows(void)
{
    // Open DB and create a table first
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "users"));

    // Insert multiple rows
    sqlk_row_t row1 = {1, "Alice"};
    sqlk_row_t row2 = {2, "Bob"};
    sqlk_row_t row3 = {3, "Charlie"};

    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row1));
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row2));
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row3));
}

void test_select_all_success(void)
{
    // Open DB and create a table first
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "users"));

    // Insert some test data
    sqlk_row_t row1 = {1, "Alice"};
    sqlk_row_t row2 = {2, "Bob"};
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row1));
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_insert_row(db, "users", &row2));

    // This will print to stdout but should return SQLK_OK
    printf("\n--- Expected output for test_select_all_success ---\n");
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_select_all(db, "users"));
    printf("--- End expected output ---\n");
}

void test_select_all_empty_table(void)
{
    // Open DB and create an empty table
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_create_table(db, "empty_table"));

    // Select from empty table should work but show 0 rows
    printf("\n--- Expected output for test_select_all_empty_table ---\n");
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_select_all(db, "empty_table"));
    printf("--- End expected output ---\n");
}

void test_select_all_null_db(void)
{
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_select_all(NULL, "users"));
}

void test_select_all_null_table(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_select_all(db, NULL));
}

void test_select_all_table_not_found(void)
{
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_open(TEST_DB_PATH, &db));
    TEST_ASSERT_NOT_NULL(db);
    
    TEST_ASSERT_EQUAL(SQLK_ERROR, sqlk_select_all(db, "nonexistent"));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_open);
    RUN_TEST(test_alloc_page);
    RUN_TEST(test_write_page);
    RUN_TEST(test_read_page);
    RUN_TEST(test_create_table_success);
    RUN_TEST(test_create_table_null_db);
    RUN_TEST(test_create_table_null_name);
    RUN_TEST(test_insert_row_success);
    RUN_TEST(test_insert_row_null_db);
    RUN_TEST(test_insert_row_null_table);
    RUN_TEST(test_insert_row_null_row);
    RUN_TEST(test_insert_row_table_not_found);
    RUN_TEST(test_insert_multiple_rows);
    RUN_TEST(test_select_all_success);
    RUN_TEST(test_select_all_empty_table);
    RUN_TEST(test_select_all_null_db);
    RUN_TEST(test_select_all_null_table);
    RUN_TEST(test_select_all_table_not_found);

    return UNITY_END();
}
