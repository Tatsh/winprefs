#include "registry.h"

static wchar_t *OUTPUT_DIR = L"\\c\\users\\name\\AppData\\Roaming\\prefs-export";

static void test_save_prefs_wfullpath_error(void **state) {
    will_return(__wrap__wfullpath, (void *)0);
    will_return(__wrap__wfullpath, FALSE);

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
    will_return(__wrap__wfullpath, TRUE);
    will_return(__wrap_CreateDirectory, FALSE);
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
    will_return(__wrap__wfullpath, TRUE);
    will_return(__wrap_CreateDirectory, TRUE);
    will_return(__wrap_CreateDirectory, TRUE);
    will_return(__wrap_CreateDirectory, TRUE);
    will_return(__wrap_CreateDirectory, TRUE);
    will_return(__wrap_CreateDirectory, FALSE);
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
    will_return(__wrap__wfullpath, TRUE);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, TRUE);
    will_return_always(__wrap_CreateDirectory, TRUE);
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
    will_return(__wrap__wfullpath, TRUE);
    will_return(__wrap_PathAppend, L"exec-reg.bat");
    will_return(__wrap_PathAppend, TRUE);
    will_return(__wrap_GetStdHandle, INVALID_HANDLE_VALUE);

    bool ret = save_preferences(
        false, nullptr, OUTPUT_DIR, L"-", 2, HKEY_CURRENT_USER, L"HKCU:\\", OUTPUT_FORMAT_REG);

    assert_false(ret);
}

const struct CMUnitTest save_prefs_tests[] = {
    cmocka_unit_test(test_save_prefs_CreateFile_returns_invalid_handle),
    cmocka_unit_test(test_save_prefs_GetStdHandle_returns_invalid_handle),
    cmocka_unit_test(test_save_prefs_fail_to_create_storage_dir),
    cmocka_unit_test(test_save_prefs_fail_to_create_storage_dir_alt),
    cmocka_unit_test(test_save_prefs_wfullpath_error),
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(save_prefs_tests, nullptr, nullptr);
}
