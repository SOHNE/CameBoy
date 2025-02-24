#include <stdio.h>
#include <stdlib.h>
#include "check.h"
#include <stdbool.h>

START_TEST(test_nothing)
{
    ck_assert_uint_eq(0, false);
}
END_TEST

START_TEST(test_truth)
{
    ck_assert_uint_eq(1, true);
}
END_TEST

START_TEST(test_addition)
{
    ck_assert_int_eq(2 + 2, 4);
}
END_TEST

START_TEST(test_string)
{
    ck_assert_str_eq("hello", "hello");
}
END_TEST

START_TEST(test_pointer)
{
    int *ptr = malloc(sizeof(int));
    ck_assert_ptr_nonnull(ptr);
    free(ptr);
}
END_TEST

Suite *stack_suite(void) {
    Suite *s = suite_create("emu");
    TCase *tc = tcase_create("core");

    // Add all test cases to the test suite
    tcase_add_test(tc, test_nothing);
    tcase_add_test(tc, test_truth);
    tcase_add_test(tc, test_addition);
    tcase_add_test(tc, test_string);
    tcase_add_test(tc, test_pointer);

    suite_add_tcase(s, tc);
    return s;
}

int main(void) {
    Suite *s = stack_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int nf = srunner_ntests_failed(sr);

    srunner_free(sr);
    return nf == 0 ? 0 : -1;
}
