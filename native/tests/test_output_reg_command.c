#include "constants.h"
#include "reg_command.h"

void test_output_reg_command_dword(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_CLASSES_ROOT\\Control Panel\\Desktop\" /v \"DoubleClickHeight\" "
                  L"/t REG_DWORD /d 4 /f",
                  95);
    will_return(__wrap_write_output, true);
    DWORD *test_val = malloc(sizeof(DWORD));
    test_val[0] = 4;
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_CLASSES_ROOT\\Control Panel\\Desktop",
                                    L"DoubleClickHeight",
                                    (const char *)test_val,
                                    sizeof(DWORD),
                                    REG_DWORD);
    assert_true(ret);
    free(test_val);
}

void test_output_reg_command_qword(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"reg add \"HKEY_LOCAL_MACHINE\\Control Panel\\Desktop\" /v \"DoubleClickHeight\" "
        L"/t REG_QWORD /d 4 /f",
        95);
    will_return(__wrap_write_output, true);
    QWORD *test_val = malloc(sizeof(QWORD));
    test_val[0] = 4;
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_LOCAL_MACHINE\\Control Panel\\Desktop",
                                    L"DoubleClickHeight",
                                    (const char *)test_val,
                                    sizeof(QWORD),
                                    REG_QWORD);
    assert_true(ret);
    free(test_val);
}

void test_output_reg_command_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_CURRENT_CONFIG\\Environment\" /v \"TEMP\" /t REG_SZ /d "
                  L"\"\"\"quoted \\string\"\" fff\" /f",
                  94);
    will_return(__wrap_write_output, true);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_CURRENT_CONFIG\\Environment",
                                    L"TEMP",
                                    (const char *)L"\"quoted \\string\" fff",
                                    21 * sizeof(wchar_t),
                                    REG_SZ);
    assert_true(ret);
}

void test_output_reg_command_expand_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_CURRENT_USER\\Environment\" /v \"TEMP\" /t REG_EXPAND_SZ /d \"a "
                  L"midsummer night's %%dream\" /f",
                  103);
    will_return(__wrap_write_output, true);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_CURRENT_USER\\Environment",
                                    L"TEMP",
                                    (const char *)L"a midsummer night's %dream%",
                                    26 * sizeof(wchar_t),
                                    REG_EXPAND_SZ);
    assert_true(ret);
}

const wchar_t *MULTI_SZ_TEST_DATA = L"\"quoted string\" fff\0test2\0\0";

void test_output_reg_command_multi_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_USERS\\Environment\" /v \"TEMP\" /t REG_MULTI_SZ /d "
                  L"\"\"\"quoted string\"\" fff\0test2\\0\" /f",
                  98);
    will_return(__wrap_write_output, true);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_USERS\\Environment",
                                    L"TEMP",
                                    (const char *)MULTI_SZ_TEST_DATA,
                                    27 * sizeof(wchar_t),
                                    REG_MULTI_SZ);
    assert_true(ret);
}

#if SIZEOF_WCHAR_T == 4
const unsigned char MULTI_SZ_TEST_DATA_INVALID[] = {
    L'e', 0, 0, 0, 'n', 0, 0, 0, '-', 0, 0, 0, 'U', 0, 0, 0, 'S'};
#else
const unsigned char MULTI_SZ_TEST_DATA_INVALID[] = {L'e', 0, 'n', 0, '-', 0, 'U', 0, 'S'};
#endif

void test_output_reg_command_multi_sz_invalid(void **state) {
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_USERS\\Environment",
                                    L"TEMP",
                                    (const char *)MULTI_SZ_TEST_DATA_INVALID,
                                    sizeof(MULTI_SZ_TEST_DATA_INVALID),
                                    REG_MULTI_SZ);
    assert_true(ret);
}

void test_output_reg_command_invalid_type(void **state) {
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_USERS\\Environment",
                                    L"TEMP",
                                    (const char *)MULTI_SZ_TEST_DATA,
                                    25 * sizeof(wchar_t),
                                    10240);
    assert_true(ret);
    assert_int_equal(errno, EINVAL);
}

void test_output_reg_command_none(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_CURRENT_USER\\Environment\" /v \"TEMP\" /t REG_NONE /f",
                  65);
    will_return(__wrap_write_output, true);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_CURRENT_USER\\Environment",
                                    L"TEMP",
                                    (const char *)MULTI_SZ_TEST_DATA,
                                    0,
                                    REG_NONE);
    assert_true(ret);
}

const unsigned char BINARY_TEST_DATA[] = {0x30, 0, 0x02, 0x80, 0x12, 0, 0, 0};

void test_output_reg_command_binary(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_DYN_DATA\\Control Panel\\Desktop\" /v \"UserPreferencesMask\" "
                  L"/t REG_BINARY /d 300002ff12000000 /f",
                  108);
    will_return(__wrap_write_output, true);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_DYN_DATA\\Control Panel\\Desktop",
                                    L"UserPreferencesMask",
                                    (const char *)BINARY_TEST_DATA,
                                    sizeof(BINARY_TEST_DATA),
                                    REG_BINARY);
    assert_true(ret);
}

void test_output_reg_command_default_key(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_DYN_DATA\\Control Panel\\Desktop\" /ve "
                  L"/t REG_SZ /d \"out\" /f",
                  108);
    will_return(__wrap_write_output, true);
    wchar_t *data = L"out";
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_DYN_DATA\\Control Panel\\Desktop",
                                    nullptr,
                                    (const char *)data,
                                    4 * sizeof(wchar_t),
                                    REG_SZ);
    assert_true(ret);

    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_DYN_DATA\\Control Panel\\Desktop\" /ve "
                  L"/t REG_SZ /d \"out\" /f",
                  108);
    will_return(__wrap_write_output, true);
    ret = do_write_reg_command(nullptr,
                               L"HKEY_DYN_DATA\\Control Panel\\Desktop",
                               L"",
                               (const char *)data,
                               4 * sizeof(wchar_t),
                               REG_SZ);
    assert_true(ret);

    expect_memory(__wrap_write_output,
                  out,
                  L"reg add \"HKEY_DYN_DATA\\Control Panel\\Desktop\" /ve "
                  L"/t REG_SZ /d \"out\" /f",
                  108);
    will_return(__wrap_write_output, true);
    ret = do_write_reg_command(nullptr,
                               L"HKEY_DYN_DATA\\Control Panel\\Desktop",
                               L"(default)",
                               (const char *)data,
                               4 * sizeof(wchar_t),
                               REG_SZ);
    assert_true(ret);
}

void test_output_reg_command_skip_too_big(void **state) {
    wchar_t *buf = calloc(CMD_MAX_COMMAND_LENGTH + 2000, sizeof(wchar_t));
    wmemset(buf, L'a', CMD_MAX_COMMAND_LENGTH + 2000);
    bool ret = do_write_reg_command(nullptr,
                                    L"HKEY_CURRENT_USER\\Control Panel\\Desktop",
                                    L"UserPreferencesMask",
                                    (const char *)buf,
                                    CMD_MAX_COMMAND_LENGTH + 2000,
                                    REG_BINARY);
    free(buf);
    assert_true(ret);
    assert_int_equal(errno, EKEYREJECTED);
}

const struct CMUnitTest output_reg_command_tests[] = {
    cmocka_unit_test(test_output_reg_command_binary),
    cmocka_unit_test(test_output_reg_command_default_key),
    cmocka_unit_test(test_output_reg_command_dword),
    cmocka_unit_test(test_output_reg_command_expand_sz),
    cmocka_unit_test(test_output_reg_command_invalid_type),
    cmocka_unit_test(test_output_reg_command_multi_sz),
#ifndef ASAN_ENABLED
    cmocka_unit_test(test_output_reg_command_multi_sz_invalid),
#endif
    cmocka_unit_test(test_output_reg_command_none),
    cmocka_unit_test(test_output_reg_command_qword),
    cmocka_unit_test(test_output_reg_command_skip_too_big),
    cmocka_unit_test(test_output_reg_command_sz),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(output_reg_command_tests, nullptr, nullptr);
}
