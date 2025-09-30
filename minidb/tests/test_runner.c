#include "unity.h"

// Forward declarations of test functions
void test_placeholder(void);

// REPL test functions
void test_parse_create_table_simple(void);
void test_parse_create_table_multiple_columns(void);
void test_parse_drop_table(void);
void test_parse_create_index(void);
void test_parse_drop_index(void);
void test_parse_insert_integers(void);
void test_parse_insert_mixed_types(void);
void test_parse_insert_text_only(void);
void test_parse_select_simple(void);
void test_parse_select_with_where_int(void);
void test_parse_select_with_where_text(void);
void test_parse_update_simple(void);
void test_parse_update_with_where(void);
void test_parse_delete_simple(void);
void test_parse_delete_with_where(void);
void test_parse_list_tables(void);
void test_parse_help(void);
void test_parse_exit(void);
void test_parse_quit(void);
void test_parse_case_insensitive(void);
void test_parse_invalid_statements(void);

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();

    // General database tests
    RUN_TEST(test_placeholder);

    // REPL parsing tests
    RUN_TEST(test_parse_create_table_simple);
    RUN_TEST(test_parse_create_table_multiple_columns);
    RUN_TEST(test_parse_drop_table);
    RUN_TEST(test_parse_create_index);
    RUN_TEST(test_parse_drop_index);
    RUN_TEST(test_parse_insert_integers);
    RUN_TEST(test_parse_insert_mixed_types);
    RUN_TEST(test_parse_insert_text_only);
    RUN_TEST(test_parse_select_simple);
    RUN_TEST(test_parse_select_with_where_int);
    RUN_TEST(test_parse_select_with_where_text);
    RUN_TEST(test_parse_update_simple);
    RUN_TEST(test_parse_update_with_where);
    RUN_TEST(test_parse_delete_simple);
    RUN_TEST(test_parse_delete_with_where);
    RUN_TEST(test_parse_list_tables);
    RUN_TEST(test_parse_help);
    RUN_TEST(test_parse_exit);
    RUN_TEST(test_parse_quit);
    RUN_TEST(test_parse_case_insensitive);
    RUN_TEST(test_parse_invalid_statements);

    return UNITY_END();
}