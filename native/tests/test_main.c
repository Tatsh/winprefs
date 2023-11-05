#include "tests.h"

int wmain(int argc, wchar_t *argv[]);

void test_main_help(void **state) {
    wchar_t **buf = calloc(2, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(10, sizeof(wchar_t));
    wmemset(buf[0], L'\0', 8);
    wmemset(buf[1], L'\0', 10);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemcpy(buf[1], L"--help", 6);
    int ret = wmain(2, buf);
    free(buf[0]);
    free(buf[1]);
    free(buf);

    assert_int_equal(ret, EXIT_SUCCESS);
}

void test_main_invalid_option(void **state) {
    wchar_t **buf = calloc(2, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(16, sizeof(wchar_t));
    wmemset(buf[0], L'\0', 8);
    wmemset(buf[1], L'\0', 16);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemcpy(buf[1], L"--invalid-option", 16);
    int ret = wmain(2, buf);
    free(buf[0]);
    free(buf[1]);
    free(buf);

    assert_int_equal(EXIT_FAILURE, ret);
}

void test_main_wfullpath_error(void **state) {
    will_return(__wrap_wSHGetFolderPathW, 0);
    will_return(__wrap__wfullpath, (void *)0);
    will_return(__wrap_wGetLastError, 0);
    will_return(__wrap_wFormatMessageW, 10);

    wchar_t **buf = calloc(1, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    int ret = wmain(1, buf);
    free(buf[0]);
    free(buf);

    assert_return_code(ret, EXIT_FAILURE);
}

void test_main_fail_to_create_storage_dir(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_wSHGetFolderPathW, app_data_folder);
    will_return(__wrap_wSHGetFolderPathW, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_wPathAppendW, prefs_export_path);
    will_return(__wrap_wPathAppendW, TRUE);
    will_return(__wrap__wfullpath, prefs_export_path);
    will_return(__wrap__wfullpath, TRUE);
    will_return(__wrap_wCreateDirectoryW, FALSE);
    will_return_always(__wrap_wGetLastError, 0);
    will_return(__wrap_wFormatMessageW, 10);

    wchar_t **buf = calloc(1, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    int ret = wmain(1, buf);

    assert_return_code(ret, EXIT_FAILURE);

    free(buf[0]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}
