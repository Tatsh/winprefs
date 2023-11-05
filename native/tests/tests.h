#ifndef TESTS_H
#define TESTS_H

void test_main_help(void **state);
void test_main_invalid_option(void **state);
void test_main_wfullpath_error(void **state);
void test_main_fail_to_create_storage_dir(void **state);

void test_escape_handles_reg_multi_sz(void **state);
void test_escape_handles_special_characters(void **state);
void test_escape_returns_null_on_null_input_or_zero_chars(void **state);
void test_escape_returns_same_when_escaping_unnecessary(void **state);

#endif // TESTS_H
