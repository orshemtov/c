#include "quick_sort.h"
#include "unity.h"
#include <string.h>

// Unity requires these functions
void setUp(void)
{
    // Set up code before each test
}

void tearDown(void)
{
    // Clean up code after each test
}

void test_quick_sort_sorted_array(void)
{
    int arr[] = {1, 2, 3, 4, 5};
    int expected[] = {1, 2, 3, 4, 5};
    quick_sort(arr, 0, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 5);
}

void test_quick_sort_reverse_array(void)
{
    int arr[] = {5, 4, 3, 2, 1};
    int expected[] = {1, 2, 3, 4, 5};
    quick_sort(arr, 0, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 5);
}

void test_quick_sort_random_array(void)
{
    int arr[] = {3, 1, 4, 5, 2};
    int expected[] = {1, 2, 3, 4, 5};
    quick_sort(arr, 0, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 5);
}

void test_quick_sort_duplicates(void)
{
    int arr[] = {2, 3, 2, 1, 3};
    int expected[] = {1, 2, 2, 3, 3};
    quick_sort(arr, 0, 4);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 5);
}

void test_quick_sort_single_element(void)
{
    int arr[] = {42};
    int expected[] = {42};
    quick_sort(arr, 0, 0);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 1);
}

void test_quick_sort_empty_array(void)
{
    int arr[1];
    quick_sort(arr, 0, -1);
    // Should not crash, nothing to assert
    TEST_ASSERT_TRUE(1); // Just to have an assertion
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_quick_sort_sorted_array);
    RUN_TEST(test_quick_sort_reverse_array);
    RUN_TEST(test_quick_sort_random_array);
    RUN_TEST(test_quick_sort_duplicates);
    RUN_TEST(test_quick_sort_single_element);
    RUN_TEST(test_quick_sort_empty_array);

    return UNITY_END();
}