#include "tests.h"

const struct CMUnitTest main_tests[] = {
    cmocka_unit_test(test_main_help),
    cmocka_unit_test(test_main_invalid_option),
    cmocka_unit_test(test_main_wfullpath_error),
    cmocka_unit_test(test_main_fail_to_create_storage_dir),
};
const struct CMUnitTest shell_tests[] = {
    cmocka_unit_test(test_escape_returns_null_on_null_input_or_zero_chars),
    cmocka_unit_test(test_escape_handles_special_characters),
    cmocka_unit_test(test_escape_handles_reg_multi_sz),
    cmocka_unit_test(test_escape_returns_same_when_escaping_unnecessary),
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    int ret0 = cmocka_run_group_tests(main_tests, nullptr, nullptr);
    int ret1 = cmocka_run_group_tests(shell_tests, nullptr, nullptr);
    return ret0 && ret1;
}
