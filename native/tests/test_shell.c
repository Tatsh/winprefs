#include "io.h"
#include "shell.h"
#include "tests.h"

void test_escape_returns_null_on_null_input_or_zero_chars(void **state) {
    (void)state;
    assert_null(escape_for_batch(nullptr, 10));
    assert_null(escape_for_batch(L"input", 0));
}

void test_escape_handles_special_characters(void **state) {
    (void)state;
    wchar_t *out = escape_for_batch(L"%VAR%", 5);
    assert_memory_equal(L"%%VAR%%", out, 7);
    free(out);

    out = escape_for_batch(L"\"VAR\"", 5);
    assert_memory_equal(L"\"\"VAR\"\"", out, 7);
    free(out);
}

void test_escape_handles_reg_multi_sz(void **state) {
    (void)state;
    wchar_t input[] = {L'a', L'\0', L'b', L'\0', L'c'};
    wchar_t *out = escape_for_batch(input, 7);
    assert_memory_equal(L"a\\0b\\0c", out, 9);
    free(out);
}

void test_escape_returns_same_when_escaping_unnecessary(void **state) {
    (void)state;
    wchar_t *out = escape_for_batch(L"abcdef", 6);
    assert_memory_equal(L"abcdef", out, 6);
    free(out);
}
