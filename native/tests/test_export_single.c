#include "registry.h"

const enum OUTPUT_FORMAT formats[] = {
    OUTPUT_FORMAT_C, OUTPUT_FORMAT_C_SHARP, OUTPUT_FORMAT_POWERSHELL, OUTPUT_FORMAT_REG};

void test_export_single_value_invalid_format(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryValueEx, ERROR_SUCCESS);
    will_return(__wrap_GetStdHandle, 0);
    bool ret = export_single_value(
        HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", OUTPUT_FORMAT_UNKNOWN);
    assert_false(ret);
    assert_int_equal(errno, EINVAL);
}

void test_export_single_value_formats(void **state) {
    will_return_always(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return_always(__wrap_RegQueryValueEx, ERROR_SUCCESS);
    will_return_always(__wrap_GetStdHandle, 0);
    will_return(__wrap_do_write_reg_command, true);
    will_return(__wrap_do_write_c_reg_code, true);
    will_return(__wrap_do_write_c_sharp_reg_code, true);
    will_return(__wrap_do_write_powershell_reg_code, true);
    size_t i;
    for (i = 0; i < 4; i++) {
        bool ret =
            export_single_value(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", formats[i]);
        assert_true(ret);
    }
}

void test_export_single_value_formats_fail(void **state) {
    will_return_always(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return_always(__wrap_RegQueryValueEx, ERROR_SUCCESS);
    will_return_always(__wrap_GetStdHandle, 0);
    will_return(__wrap_do_write_reg_command, false);
    will_return(__wrap_do_write_c_reg_code, false);
    will_return(__wrap_do_write_c_sharp_reg_code, false);
    will_return(__wrap_do_write_powershell_reg_code, false);
    size_t i;
    for (i = 0; i < 4; i++) {
        bool ret =
            export_single_value(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", formats[i]);
        assert_false(ret);
    }
}

void test_export_single_value_RegOpenKeyEx_fail(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_ACCESS_DENIED);
    bool ret =
        export_single_value(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", OUTPUT_FORMAT_REG);
    assert_false(ret);
}

void test_export_single_value_RegQueryValueEx_too_large(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryValueEx, ERROR_MORE_DATA);
    bool ret =
        export_single_value(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", OUTPUT_FORMAT_REG);
    assert_false(ret);
}

void test_export_single_value_RegQueryValueEx_fail(void **state) {
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryValueEx, ERROR_ACCESS_DENIED);
    bool ret =
        export_single_value(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\\A\\B\\C", OUTPUT_FORMAT_REG);
    assert_false(ret);
}

void test_export_single_value_no_backslash(void **state) {
    bool ret = export_single_value(HKEY_CURRENT_USER, L"", OUTPUT_FORMAT_REG);
    assert_false(ret);
    assert_int_equal(errno, EINVAL);
}

void test_export_single_value_null_reg_path(void **state) {
    bool ret = export_single_value(HKEY_CURRENT_USER, nullptr, OUTPUT_FORMAT_REG);
    assert_false(ret);
    assert_int_equal(errno, EINVAL);
}

const struct CMUnitTest export_single_value_tests[] = {
    cmocka_unit_test(test_export_single_value_RegOpenKeyEx_fail),
    cmocka_unit_test(test_export_single_value_RegQueryValueEx_fail),
    cmocka_unit_test(test_export_single_value_RegQueryValueEx_too_large),
    cmocka_unit_test(test_export_single_value_formats),
    cmocka_unit_test(test_export_single_value_formats_fail),
    cmocka_unit_test(test_export_single_value_no_backslash),
    cmocka_unit_test(test_export_single_value_null_reg_path),
    cmocka_unit_test(test_export_single_value_invalid_format),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(export_single_value_tests, nullptr, nullptr);
}
