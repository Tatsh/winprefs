#include "calloc.h"
#include "cmocka-inc.h"

#include "tests.h"

bool want_calloc_abort = false;

const struct CMUnitTest shell_tests[] = {
    cmocka_unit_test(test_escape_returns_null_on_null_input_or_zero_chars),
    cmocka_unit_test(test_escape_handles_special_characters),
    cmocka_unit_test(test_escape_handles_reg_multi_sz),
    cmocka_unit_test(test_escape_returns_same_when_escaping_unnecessary),
//    cmocka_unit_test(test_escape_aborts_if_calloc_fails),
};

int wmain(int argc, wchar_t *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(shell_tests, NULL, NULL);
}
