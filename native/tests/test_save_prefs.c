#include "constants.h"
#include "registry.h"

static wchar_t *OUTPUT_DIR = L"\\c\\users\\name\\AppData\\Roaming\\prefs-export";

static void test_save_prefs_wfullpath_error(void **state) {
    will_return(__wrap__wfullpath, (void *)0);
    will_return(__wrap__wfullpath, false);

    bool ret = save_preferences(false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG);

    assert_false(ret);
}

static void test_save_prefs_fail_to_create_storage_dir(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_CreateDirectory, false);
    will_return(__wrap_GetLastError, 999);

    bool ret = save_preferences(false,
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG);

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
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG);

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
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_CURRENT_USER,
                                L"HKCU:\\",
                                OUTPUT_FORMAT_REG);

    assert_false(ret);
}

void test_save_prefs_GetStdHandle_returns_invalid_handle(void **state) {
    will_return(__wrap__wfullpath, OUTPUT_DIR);
    will_return(__wrap__wfullpath, true);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, true);
    will_return(__wrap_GetStdHandle, INVALID_HANDLE_VALUE);

    bool ret = save_preferences(
        false, nullptr, OUTPUT_DIR, L"-", 2, HKEY_CURRENT_USER, L"HKCU:\\", OUTPUT_FORMAT_REG);

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

    bool ret = save_preferences(
        false, nullptr, OUTPUT_DIR, L"-", 2, HKEY_DYN_DATA, L"HKDD:\\", OUTPUT_FORMAT_C);

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
                                nullptr,
                                OUTPUT_DIR,
                                L"exec-reg.bat",
                                2,
                                HKEY_DYN_DATA,
                                L"HKDD:\\",
                                OUTPUT_FORMAT_REG);

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
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(save_prefs_tests, nullptr, nullptr);
}
