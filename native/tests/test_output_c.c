#include "constants.h"
#include "reg_code.h"

void test_output_c_null_escaped_reg_key(void **state) {
    DWORD *test_val = malloc(sizeof(DWORD));
    test_val[0] = 4;
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_CURRENT_USER", // invalid argument
                                   L"DoubleClickHeight",
                                   (const char *)test_val,
                                   sizeof(DWORD),
                                   REG_DWORD);
    assert_false(ret);
    free(test_val);
}

void test_output_c_dword(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"dnum = 4; RegSetKeyValue(HKEY_CLASSES_ROOT, TEXT(\"Control Panel\\Desktop\"), "
                  L"TEXT(\"DoubleClickHeight\"), REG_DWORD, (LPCVOID)&dnum, sizeof(DWORD));",
                  64 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    DWORD *test_val = malloc(sizeof(DWORD));
    test_val[0] = 4;
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_CLASSES_ROOT\\Control Panel\\Desktop",
                                   L"DoubleClickHeight",
                                   (const char *)test_val,
                                   sizeof(DWORD),
                                   REG_DWORD);
    assert_true(ret);
    free(test_val);
}

void test_output_c_qword(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"qnum = 4; RegSetKeyValue(HKEY_LOCAL_MACHINE, TEXT(\"Control Panel\\Desktop\"), "
                  L"TEXT(\"DoubleClickHeight\"), REG_QWORD, (LPCVOID)&qnum, sizeof(QWORD));",
                  64 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    QWORD *test_val = malloc(sizeof(QWORD));
    test_val[0] = 4;
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_LOCAL_MACHINE\\Control Panel\\Desktop",
                                   L"DoubleClickHeight",
                                   (const char *)test_val,
                                   sizeof(QWORD),
                                   REG_QWORD);
    assert_true(ret);
    free(test_val);
}

void test_output_c_sz(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"RegSetKeyValue(HKEY_CURRENT_CONFIG, TEXT(\"Environment\"), TEXT(\"TEMP\"), REG_SZ, "
        L"TEXT(\"\\\"quoted \\\\string\\\" fff\"), 104);",
        50 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_CURRENT_CONFIG\\Environment",
                                   L"TEMP",
                                   (const char *)L"\"quoted \\string\" fff",
                                   26 * sizeof(wchar_t),
                                   REG_SZ);
    assert_true(ret);
}

void test_output_c_expand_sz(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"RegSetKeyValue(HKEY_CURRENT_USER, TEXT(\"Environment\"), TEXT(\"TEMP\"), REG_EXPAND_SZ, "
        L"TEXT(\"a midsummer night's %dream%\"), 52);",
        50 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_CURRENT_USER\\Environment",
                                   L"TEMP",
                                   (const char *)L"a midsummer night's %dream%",
                                   26 * sizeof(wchar_t),
                                   REG_EXPAND_SZ);
    assert_true(ret);
}

const wchar_t *MULTI_SZ_TEST_DATA = L"\"quoted string\" fff\0test2\0\0";

void test_output_c_multi_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"RegSetKeyValue(HKEY_USERS, TEXT(\"Environment\"), TEXT(\"TEMP\"), "
                  L"REG_MULTI_SZ, TEXT(\"\\\"quoted string\\\" fff\\0test2\\0\\0\"), 54);",
                  50 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_USERS\\Environment",
                                   L"TEMP",
                                   (const char *)MULTI_SZ_TEST_DATA,
                                   28 * sizeof(wchar_t),
                                   REG_MULTI_SZ);
    assert_true(ret);
}

const wchar_t *MULTI_SZ_TEST_DATA_INVALID = L"\"quoted string\" fff\0test2";

void test_output_c_multi_sz_invalid(void **state) {
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_USERS\\Environment",
                                   L"TEMP",
                                   (const char *)MULTI_SZ_TEST_DATA,
                                   25 * sizeof(wchar_t),
                                   REG_MULTI_SZ);
    assert_true(ret);
}

void test_output_c_top_key_invalid(void **state) {
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_USER_Z\\Environment",
                                   L"TEMP",
                                   (const char *)MULTI_SZ_TEST_DATA,
                                   25 * sizeof(wchar_t),
                                   REG_MULTI_SZ);
    assert_false(ret);
}

void test_output_c_invalid_type(void **state) {
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_USERS\\Environment",
                                   L"TEMP",
                                   (const char *)MULTI_SZ_TEST_DATA,
                                   25 * sizeof(wchar_t),
                                   10240);
    assert_true(ret);
    assert_int_equal(errno, EINVAL);
}

void test_output_c_none(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"RegSetKeyValue(HKEY_CURRENT_USER, TEXT(\"Environment\"), TEXT(\"TEMP\"), "
                  L"REG_NONE, NULL, 0);",
                  89 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_CURRENT_USER\\Environment",
                                   L"TEMP",
                                   (const char *)MULTI_SZ_TEST_DATA,
                                   0,
                                   REG_NONE);
    assert_true(ret);
}

const unsigned char BINARY_TEST_DATA[] = {0x30, 0, 0x02, 0x80, 0x12, 0, 0, 0};

void test_output_c_binary(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"data = { 0x30, 0x00, 0x02, 0x80, 0x12, 0x00, 0x00, 0x00 }; "
                  L"RegSetKeyValue(HKEY_DYN_DATA, TEXT(\"Control Panel\\\\Desktop\"), "
                  L"TEXT(\"UserPreferencesMask\"), REG_BINARY, (LPCVOID)&data, 8);",
                  182 * sizeof(wchar_t));
    will_return(__wrap_write_output, true);
    bool ret = do_write_c_reg_code(nullptr,
                                   L"HKEY_DYN_DATA\\Control Panel\\Desktop",
                                   L"UserPreferencesMask",
                                   (const char *)BINARY_TEST_DATA,
                                   sizeof(BINARY_TEST_DATA),
                                   REG_BINARY);
    assert_true(ret);
}

const struct CMUnitTest output_c_tests[] = {
    cmocka_unit_test(test_output_c_binary),
    cmocka_unit_test(test_output_c_dword),
    cmocka_unit_test(test_output_c_expand_sz),
    cmocka_unit_test(test_output_c_multi_sz),
    cmocka_unit_test(test_output_c_multi_sz_invalid),
    cmocka_unit_test(test_output_c_none),
    cmocka_unit_test(test_output_c_null_escaped_reg_key),
    cmocka_unit_test(test_output_c_qword),
    cmocka_unit_test(test_output_c_sz),
    cmocka_unit_test(test_output_c_top_key_invalid),
    cmocka_unit_test(test_output_c_invalid_type),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(output_c_tests, nullptr, nullptr);
}
