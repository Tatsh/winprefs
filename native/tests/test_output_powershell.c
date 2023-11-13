#include "constants.h"
#include "powershell.h"

void test_powershell_null_escaped_reg_key(void **state) {
    DWORD *test_val = malloc(sizeof(DWORD));
    test_val[0] = 4;
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER", // invalid argument
                                            L"DoubleClickHeight",
                                            (const char *)test_val,
                                            sizeof(DWORD),
                                            REG_DWORD);
    assert_false(ret);
    free(test_val);
}

void test_powershell_dword(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Control Panel\\Desktop')) { New-Item -Path "
        L"'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Force | Out-Null } "
        L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Name "
        L"'DoubleClickHeight' -PropertyType DWord -Force -Value 4",
        275);
    will_return(__wrap_write_output, true);
    DWORD *test_val = malloc(sizeof(DWORD));
    test_val[0] = 4;
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Control Panel\\Desktop",
                                            L"DoubleClickHeight",
                                            (const char *)test_val,
                                            sizeof(DWORD),
                                            REG_DWORD);
    assert_true(ret);
    free(test_val);
}

void test_powershell_qword(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Control Panel\\Desktop')) { New-Item -Path "
        L"'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Force | Out-Null } "
        L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Name "
        L"'DoubleClickHeight' -PropertyType QWord -Force -Value 4",
        275);
    will_return(__wrap_write_output, true);
    QWORD *test_val = malloc(sizeof(QWORD));
    test_val[0] = 4;
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Control Panel\\Desktop",
                                            L"DoubleClickHeight",
                                            (const char *)test_val,
                                            sizeof(QWORD),
                                            REG_QWORD);
    assert_true(ret);
    free(test_val);
}

void test_powershell_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Environment')) { New-Item -Path "
                  L"'HKEY_CURRENT_USER:\\Environment' -Force | Out-Null } "
                  L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Environment' -Name "
                  L"'TEMP' -PropertyType String -Force -Value 'a midsummer night''s dream'",
                  275);
    will_return(__wrap_write_output, true);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Environment",
                                            L"TEMP",
                                            (const char *)L"a midsummer night's dream",
                                            26 * sizeof(wchar_t),
                                            REG_SZ);
    assert_true(ret);
}

void test_powershell_expand_sz(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Environment')) { New-Item -Path "
                  L"'HKEY_CURRENT_USER:\\Environment' -Force | Out-Null } "
                  L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Environment' -Name "
                  L"'TEMP' -PropertyType ExpandString -Force -Value 'a midsummer night''s %dream%'",
                  275);
    will_return(__wrap_write_output, true);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Environment",
                                            L"TEMP",
                                            (const char *)L"a midsummer night's %dream%",
                                            26 * sizeof(wchar_t),
                                            REG_EXPAND_SZ);
    assert_true(ret);
}

const wchar_t *MULTI_SZ_TEST_DATA[] = {L"a midsummer night's dream", L"test2"};

void test_powershell_multi_sz(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Environment')) { New-Item -Path "
        L"'HKEY_CURRENT_USER:\\Environment' -Force | Out-Null } "
        L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Environment' -Name "
        L"'TEMP' -PropertyType String -Force -Value @\"\na midsummer night''s dream\ntest2\n)\n\"@",
        271);
    will_return(__wrap_write_output, true);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Environment",
                                            L"TEMP",
                                            (const char *)MULTI_SZ_TEST_DATA,
                                            31 * sizeof(wchar_t),
                                            REG_MULTI_SZ);
    assert_true(ret);
}

void test_powershell_none(void **state) {
    expect_memory(__wrap_write_output,
                  out,
                  L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Environment')) { New-Item -Path "
                  L"'HKEY_CURRENT_USER:\\Environment' -Force | Out-Null } "
                  L"New-ItemProperty -LiteralPath 'HKEY_CURRENT_USER:\\Environment' -Name "
                  L"'TEMP' -PropertyType None -Force -Value $null",
                  237);
    will_return(__wrap_write_output, true);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Environment",
                                            L"TEMP",
                                            (const char *)MULTI_SZ_TEST_DATA,
                                            0,
                                            REG_NONE);
    assert_true(ret);
}

const int BINARY_TEST_DATA[] = {0x30, 0, 0x02, 0x80, 0x12, 0, 0, 0};

void test_powershell_binary(void **state) {
    expect_memory(
        __wrap_write_output,
        out,
        L"if (!(Test-Path 'HKEY_CURRENT_USER:\\Control Panel\\Desktop')) { New-Item -Path "
        L"'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Force | Out-Null } New-ItemProperty "
        L"-LiteralPath 'HKEY_CURRENT_USER:\\Control Panel\\Desktop' -Name 'UserPreferencesMask' "
        L"-PropertyType Binary -Force -Value "
        L"(byte[]](0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x80,0x00,0x00,0x00,"
        L"0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0)",
        445);
    will_return(__wrap_write_output, true);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Control Panel\\Desktop",
                                            L"UserPreferencesMask",
                                            (const char *)BINARY_TEST_DATA,
                                            sizeof(BINARY_TEST_DATA),
                                            REG_BINARY);
    assert_true(ret);
}

void test_powershell_skip_too_big(void **state) {
    wchar_t *buf = calloc(POWERSHELL_MAX_COMMAND_LENGTH + 500, sizeof(wchar_t));
    wmemset(buf, L'a', POWERSHELL_MAX_COMMAND_LENGTH + 500);
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_CURRENT_USER\\Control Panel\\Desktop",
                                            L"UserPreferencesMask",
                                            (const char *)buf,
                                            POWERSHELL_MAX_COMMAND_LENGTH + 500,
                                            REG_BINARY);
    assert_true(ret);
    free(buf);
}

const wchar_t *MULTI_SZ_TEST_DATA_INVALID = L"\"quoted string\" fff\0test2";

void test_powershell_multi_sz_invalid(void **state) {
    bool ret = do_write_powershell_reg_code(nullptr,
                                            L"HKEY_USERS\\Environment",
                                            L"TEMP",
                                            (const char *)MULTI_SZ_TEST_DATA,
                                            25 * sizeof(wchar_t),
                                            REG_MULTI_SZ);
    assert_false(ret);
}

const struct CMUnitTest powershell_tests[] = {
    cmocka_unit_test(test_powershell_binary),
    cmocka_unit_test(test_powershell_dword),
    cmocka_unit_test(test_powershell_expand_sz),
    cmocka_unit_test(test_powershell_multi_sz),
    cmocka_unit_test(test_powershell_multi_sz_invalid),
    cmocka_unit_test(test_powershell_none),
    cmocka_unit_test(test_powershell_null_escaped_reg_key),
    cmocka_unit_test(test_powershell_qword),
    cmocka_unit_test(test_powershell_skip_too_big),
    cmocka_unit_test(test_powershell_sz),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(powershell_tests, nullptr, nullptr);
}
