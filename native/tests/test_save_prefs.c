#include "constants.h"
#include "registry.h"

static wchar_t *OUTPUT_DIR = L"\\c\\users\\name\\AppData\\Roaming\\prefs-export";

static void test_save_prefs_wfullpath_error(void **state) {
    will_return(__wrap__wfullpath, (void *)0);
    will_return(__wrap__wfullpath, false);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_false(ret);
}

static void test_save_prefs_fail_to_create_storage_dir(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_CreateDirectory, false);
    will_return(__wrap_GetLastError, 999);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_false(ret);
}

static void test_save_prefs_fail_to_create_storage_dir_alt(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateDirectory, false);
    will_return(__wrap_GetLastError, 999);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_false(ret);
}

static void test_save_prefs_CreateFile_returns_invalid_handle(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return_always(__wrap_CreateDirectory, true);
    will_return(__wrap_CreateFile, INVALID_HANDLE_VALUE);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_false(ret);
}

void test_save_prefs_GetStdHandle_returns_invalid_handle(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return(__wrap_GetStdHandle, INVALID_HANDLE_VALUE);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"-",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_false(ret);
}

void test_save_prefs_c_preamble(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return(__wrap_GetStdHandle, STD_OUTPUT_HANDLE);
    expect_value(__wrap_WriteFile, nNumberOfBytesToWrite, SIZEOF_C_PREAMBLE);
    will_return(__wrap_WriteFile, 10);
    will_return(__wrap_WriteFile, true);
    will_return(__wrap_write_key_filtered_recursive, true);

    bool ret = save_preferences(false,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"-",
                                2,
                                HKEY_DYN_DATA,
                                L"HKDD:\\",
                                OUTPUT_FORMAT_C,
                                nullptr);

    assert_true(ret);
}

void test_save_prefs_commit(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return_always(__wrap_CreateDirectory, true);
    will_return(__wrap_write_key_filtered_recursive, true);
    will_return(__wrap_CreateFile, (HANDLE)1);
    will_return(__wrap_CloseHandle, true);
    will_return(__wrap_git_commit, true);

    bool ret = save_preferences(true,
                                false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_DYN_DATA,
                                L"HKDD:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_true(ret);
}

void test_save_prefs_read_settings(void **state) {
    will_return(__wrap_AllocateAndInitializeSid, true);
    will_return(__wrap_CheckTokenMembership, true);
    will_return(__wrap_CheckTokenMembership, true);

    will_return(__wrap_RegOpenKeyEx, -1);
    expect_memory(__wrap_RegCreateKeyEx,
                  lpSubKey,
                  L"Software\\Tatsh\\WinPrefs\\Filters",
                  sizeof(L"Software\\Tatsh\\WinPrefs\\Filters"));
    will_return(__wrap_RegCreateKeyEx, ERROR_SUCCESS);
    will_return_always(__wrap_RegSetValueEx, ERROR_SUCCESS);

    const wchar_t *filter_str = L"Some\\Filter";
    will_return(__wrap_RegOpenKeyEx, ERROR_SUCCESS);
    will_return(__wrap_RegQueryInfoKey, 2);
    will_return(__wrap_RegQueryInfoKey, 128);
    will_return(__wrap_RegQueryInfoKey, ERROR_SUCCESS);
    will_return(__wrap_RegEnumValue, REG_SZ);
    will_return(__wrap_RegEnumValue, ERROR_SUCCESS);
    will_return(__wrap_RegEnumValue, REG_BINARY);
    will_return(__wrap_RegEnumValue, ERROR_SUCCESS);
    will_return(__wrap_RegEnumValue, REG_SZ);
    will_return(__wrap_RegEnumValue, filter_str);
    will_return(__wrap_RegEnumValue, 24);
    will_return(__wrap_RegEnumValue, ERROR_SUCCESS);
    will_return_always(__wrap_RegCloseKey, ERROR_SUCCESS);

    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return_always(__wrap_CreateDirectory, true);
    will_return(__wrap_write_key_filtered_recursive, true);
    will_return(__wrap_CreateFile, (HANDLE)1);
    will_return(__wrap_CloseHandle, true);

    bool ret = save_preferences(false,
                                true,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_DYN_DATA,
                                L"HKDD:\\",
                                OUTPUT_FORMAT_REG,
                                nullptr);

    assert_true(ret);
}

const struct CMUnitTest save_prefs_tests[] = {
    cmocka_unit_test(test_save_prefs_CreateFile_returns_invalid_handle),
    cmocka_unit_test(test_save_prefs_GetStdHandle_returns_invalid_handle),
    cmocka_unit_test(test_save_prefs_c_preamble),
    cmocka_unit_test(test_save_prefs_fail_to_create_storage_dir),
    cmocka_unit_test(test_save_prefs_fail_to_create_storage_dir_alt),
    cmocka_unit_test(test_save_prefs_wfullpath_error),
    cmocka_unit_test(test_save_prefs_commit),
    cmocka_unit_test(test_save_prefs_read_settings),
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(save_prefs_tests, nullptr, nullptr);
}
