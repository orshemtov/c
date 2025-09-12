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
    uint32_t pgno;
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_alloc_page(db, &pgno));
    TEST_ASSERT_EQUAL(1, pgno); // First allocated page should be page 1
}

void test_read_page(void)
{
    sqlk_page_t page;
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_read_page(db, 1, &page));
}

void test_write_page(void)
{
    sqlk_page_t page;
    // Initialize page with some data
    for (int i = 0; i < SQLK_PAGE_SIZE; i++)
    {
        ((char *)&page)[i] = (char)i;
    }
    TEST_ASSERT_EQUAL(SQLK_OK, sqlk_write_page(db, 1, &page));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_open);
    RUN_TEST(test_alloc_page);
    RUN_TEST(test_write_page);
    RUN_TEST(test_read_page);

    return UNITY_END();
}