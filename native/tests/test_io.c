#include "default_writer_setup.h"
#include "io.h"
#include "io_default_writer.h"

void test_write_output_returns_false_when_WideCharToMultiByte_returns_req_size_0(void **state) {
    will_return(__wrap_WideCharToMultiByte, 0);
    assert_false(write_output(nullptr, false, &default_writer));
}

void test_write_output_returns_true_when_WideCharToMultiByte_returns_0(void **state) {
    will_return(__wrap_WideCharToMultiByte, 10);
    will_return(__wrap_WideCharToMultiByte, 0);
    assert_true(write_output(nullptr, false, &default_writer));
}

void test_write_output_adds_cr(void **state) {
    expect_value(__wrap_WriteFile, nNumberOfBytesToWrite, 11);
    will_return(__wrap_WideCharToMultiByte, 10);
    will_return(__wrap_WideCharToMultiByte, 10);
    will_return(__wrap_WriteFile, 11);
    will_return(__wrap_WriteFile, true);
    assert_true(write_output(nullptr, true, &default_writer));
}

void test_write_key_filtered_recursive_does_not_exceed_max_depth_inclusive(void **state) {
    assert_true(write_key_filtered_recursive(
        nullptr, nullptr, 20, 20, nullptr, OUTPUT_FORMAT_REG, &default_writer));
    assert_int_equal(errno, EDOM);
}

void test_write_key_filtered_recursive_does_not_exceed_length_limitations(void **state) {
    assert_true(write_key_filtered_recursive(
        nullptr,
        L"HKCR\\Software\\Microsoft\\Windows\\CurrentVersion\\Microsoft\\Microsoft\\Microsoft"
        L"\\Microsoft\\Microsoft\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows"
        L"\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows"
        L"\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows\\Windows"
        L"\\Windows\\Windows",
        20,
        4,
        L"HKCR\\Software\\Microsoft\\Windows\\Windows\\CurrentVersion\\Windows\\CurrentVersion"
        L"\\Windows\\CurrentVersion\\Windows\\CurrentVersion",
        OUTPUT_FORMAT_REG,
        &default_writer));
    assert_int_equal(errno, E2BIG);
}

void test_write_key_filtered_has_filters(void **state) {
    assert_true(write_key_filtered_recursive(
        nullptr, L"MuiCache", 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
    assert_int_equal(errno, EKEYREJECTED);
}

void test_write_key_filtered_RegOpenKeyEx_fail(void **state) {
    will_return(__wrap_RegOpenKeyEx, -1);
    assert_true(write_key_filtered_recursive(
        nullptr, nullptr, 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
}

void test_write_key_filtered_RegQueryInfoKey_fail(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 0);
    will_return(__wrap_RegQueryInfoKey, -1);
    assert_false(write_key_filtered_recursive(
        nullptr, nullptr, 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
}

void test_write_key_filtered_RegQueryInfoKey_0_subkeys_returns_true(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 0);
    will_return(__wrap_RegQueryInfoKey, ERROR_SUCCESS);
    will_return(__wrap_RegCloseKey, 0);
    assert_true(write_key_filtered_recursive(
        nullptr, nullptr, 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
}

void test_write_key_filtered_RegEnumKeyEx_fail_returns_true(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 1);
    will_return(__wrap_RegQueryInfoKey, ERROR_SUCCESS);
    will_return(__wrap_RegEnumKeyEx, 0);
    will_return(__wrap_RegEnumKeyEx, 0);
    will_return(__wrap_RegEnumKeyEx, -1);
    will_return(__wrap_RegCloseKey, 0);
    assert_true(write_key_filtered_recursive(
        nullptr, nullptr, 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
}

void test_write_key_filtered_recursion_fail(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 1);
    will_return(__wrap_RegQueryInfoKey, ERROR_SUCCESS);
    will_return(__wrap_RegEnumKeyEx, L"main");
    will_return(__wrap_RegEnumKeyEx, 4);
    will_return(__wrap_RegEnumKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 0);
    will_return(__wrap_RegQueryInfoKey, -1);
    assert_false(write_key_filtered_recursive(
        nullptr, nullptr, 20, 4, L"HKCR\\Windows\\Shell", OUTPUT_FORMAT_REG, &default_writer));
}

void test_do_writes_n_values_0_returns_true(void **state) {
    assert_true(do_writes(nullptr, 0, nullptr, OUTPUT_FORMAT_REG, &default_writer));
}

void test_do_writes_nullptr_args(void **state) {
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_REG, nullptr));
    assert_false(do_writes(nullptr, 1, nullptr, OUTPUT_FORMAT_REG, nullptr));
}

void test_do_writes_unknown_format_arg(void **state) {
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_UNKNOWN, &default_writer));
}

void test_do_writes_callbacks_fail(void **state) {
    will_return_always(__wrap_RegEnumValue, ERROR_SUCCESS);
    will_return(__wrap_do_write_reg_command, false);
    will_return(__wrap_do_write_c_reg_code, false);
    will_return(__wrap_do_write_c_sharp_reg_code, false);
    will_return(__wrap_do_write_powershell_reg_code, false);
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_C, &default_writer));
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_C_SHARP, &default_writer));
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_POWERSHELL, &default_writer));
    assert_false(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_REG, &default_writer));
}

void test_do_writes(void **state) {
    will_return_always(__wrap_RegEnumValue, ERROR_SUCCESS);
    will_return(__wrap_do_write_c_reg_code, true);
    assert_true(do_writes(nullptr, 1, L"full path", OUTPUT_FORMAT_C, &default_writer));
}

const struct CMUnitTest io_tests[] = {
    cmocka_unit_test(test_do_writes),
    cmocka_unit_test(test_do_writes_callbacks_fail),
    cmocka_unit_test(test_do_writes_n_values_0_returns_true),
    cmocka_unit_test(test_do_writes_nullptr_args),
    cmocka_unit_test(test_do_writes_unknown_format_arg),
    cmocka_unit_test(test_write_key_filtered_RegEnumKeyEx_fail_returns_true),
    cmocka_unit_test(test_write_key_filtered_RegOpenKeyEx_fail),
    cmocka_unit_test(test_write_key_filtered_RegQueryInfoKey_0_subkeys_returns_true),
    cmocka_unit_test(test_write_key_filtered_RegQueryInfoKey_fail),
    cmocka_unit_test(test_write_key_filtered_has_filters),
    cmocka_unit_test(test_write_key_filtered_recursion_fail),
    cmocka_unit_test(test_write_key_filtered_recursive_does_not_exceed_length_limitations),
    cmocka_unit_test(test_write_key_filtered_recursive_does_not_exceed_max_depth_inclusive),
    cmocka_unit_test(test_write_output_adds_cr),
    cmocka_unit_test(test_write_output_returns_true_when_WideCharToMultiByte_returns_0),
    cmocka_unit_test(test_write_output_returns_false_when_WideCharToMultiByte_returns_req_size_0),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(io_tests, default_writer_setup_stdout, default_writer_teardown);
}
