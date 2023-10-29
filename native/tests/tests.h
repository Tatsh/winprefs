#ifndef TESTS_H
#define TESTS_H

void test_escape_handles_reg_multi_sz(void **state);
void test_escape_handles_special_characters(void **state);
void test_escape_returns_null_on_null_input_or_zero_chars(void **state);
void test_escape_returns_same_when_escaping_unnecessary(void **state);

#endif // TESTS_H
