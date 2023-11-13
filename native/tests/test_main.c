#include "io.h"

int wmain(int argc, wchar_t *argv[]);

static void test_main_help(void **state) {
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

static void test_main_invalid_option(void **state) {
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

static void test_main_reg_path_invalid(void **state) {
    wchar_t **buf = calloc(3, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(8, sizeof(wchar_t));
    buf[2] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 7);
    wmemcpy(buf[1], L"ZZZZ:\\", 7);
    int ret = wmain(2, buf);

    assert_return_code(ret, EXIT_FAILURE);

    free(buf[1]);
    free(buf[0]);
    free(buf);
}

static void test_main_reg_path_invalid_alt(void **state) {
    wchar_t **buf = calloc(3, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(6, sizeof(wchar_t));
    buf[2] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 6);
    wmemcpy(buf[1], L"ZZZZ:", 5);
    int ret = wmain(2, buf);

    assert_return_code(ret, EXIT_FAILURE);

    free(buf[1]);
    free(buf[0]);
    free(buf);
}

static void test_main_max_depth_conversion(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_value(__wrap_save_preferences, max_depth, 50);
    expect_any(__wrap_save_preferences, commit);
    expect_any(__wrap_save_preferences, format);
    expect_any(__wrap_save_preferences, deploy_key);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_dir);
    expect_any(__wrap_save_preferences, output_file);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(3, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-m", 2);
    wmemset(buf[2], L'\0', 3);
    wmemcpy(buf[2], L"50", 2);
    int ret = wmain(3, buf);

    assert_return_code(ret, EXIT_SUCCESS);

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static void test_main_commit_arg(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any(__wrap_save_preferences, max_depth);
    expect_value(__wrap_save_preferences, commit, true);
    expect_any(__wrap_save_preferences, format);
    expect_any(__wrap_save_preferences, deploy_key);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_dir);
    expect_any(__wrap_save_preferences, output_file);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(3, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-c", 2);
    int ret = wmain(2, buf);

    assert_return_code(ret, EXIT_SUCCESS);

    free(buf[0]);
    free(buf[1]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static void test_main_output_dir_arg(void **state) {
    expect_any(__wrap_save_preferences, max_depth);
    expect_any(__wrap_save_preferences, commit);
    expect_any(__wrap_save_preferences, format);
    expect_any(__wrap_save_preferences, deploy_key);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_file);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(7, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-o", 2);
    wmemset(buf[2], L'\0', 7);
    wmemcpy(buf[2], L"output", 6);
    expect_memory(__wrap_save_preferences, output_dir, buf[2], 6 * sizeof(wchar_t));
    int ret = wmain(3, buf);

    assert_return_code(ret, EXIT_SUCCESS);

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
}

static void test_main_debug_arg(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any(__wrap_save_preferences, max_depth);
    expect_any(__wrap_save_preferences, commit);
    expect_any(__wrap_save_preferences, format);
    expect_any(__wrap_save_preferences, deploy_key);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_dir);
    expect_any(__wrap_save_preferences, output_file);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(3, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-d", 2);
    debug_print_enabled = false;
    int ret = wmain(2, buf);

    assert_return_code(ret, EXIT_SUCCESS);
    assert_true(debug_print_enabled);
    debug_print_enabled = false;

    free(buf[0]);
    free(buf[1]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

void test_main_deploy_key_arg(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any(__wrap_save_preferences, max_depth);
    expect_any(__wrap_save_preferences, commit);
    expect_any(__wrap_save_preferences, format);
    expect_memory(__wrap_save_preferences, deploy_key, L"file.key", 8);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_dir);
    expect_any(__wrap_save_preferences, output_file);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(9, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-K", 2);
    wmemset(buf[2], L'\0', 9);
    wmemcpy(buf[2], L"file.key", 8);
    int ret = wmain(3, buf);

    assert_return_code(ret, EXIT_SUCCESS);
    debug_print_enabled = false;

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

void test_main_output_file_arg(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any(__wrap_save_preferences, max_depth);
    expect_any(__wrap_save_preferences, commit);
    expect_any(__wrap_save_preferences, format);
    expect_any(__wrap_save_preferences, deploy_key);
    expect_any(__wrap_save_preferences, hk);
    expect_any(__wrap_save_preferences, output_dir);
    expect_memory(__wrap_save_preferences, output_file, L"exec.reg", 8);
    expect_any(__wrap_save_preferences, specified_path);
    will_return(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(9, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-f", 2);
    wmemset(buf[2], L'\0', 9);
    wmemcpy(buf[2], L"exec.reg", 8);
    int ret = wmain(3, buf);

    assert_return_code(ret, EXIT_SUCCESS);
    debug_print_enabled = false;

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static const wchar_t *C_SHARP_FORMAT_NAMES[] = {L"cs", L"c#", L"csharp"};

static void test_main_format_arg_c_sharp(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, true);

    long unsigned int i;
    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(7, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-F", 2);
    for (i = 0; i < ARRAY_SIZE(C_SHARP_FORMAT_NAMES); i++) {
        will_return(__wrap_SHGetFolderPath, app_data_folder);
        will_return(__wrap_SHGetFolderPath, 0);
        will_return(__wrap_PathAppend, prefs_export_path);
        will_return(__wrap_PathAppend, true);

        expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_C_SHARP);
        wmemset(buf[2], L'\0', 7);
        wmemcpy(buf[2], C_SHARP_FORMAT_NAMES[i], wcslen(C_SHARP_FORMAT_NAMES[i]));
        int ret = wmain(3, buf);
        assert_return_code(ret, EXIT_SUCCESS);
    }

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static void test_main_format_arg_invalid(void **state) {
    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(8, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-F", 2);
    wmemset(buf[2], L'\0', 8);
    wmemcpy(buf[2], L"unknown", 7);
    int ret = wmain(3, buf);
    assert_return_code(ret, EXIT_FAILURE);

    free(buf[0]);
    free(buf[1]);
    free(buf);
}

static const wchar_t *FULL_REG_PATH_SUFFIX = L":\\Software\\Microsoft\\Windows NT\\some value name";
static const HKEY possible_hkey[] = {HKEY_CLASSES_ROOT,
                                     HKEY_CURRENT_CONFIG,
                                     HKEY_CURRENT_USER,
                                     HKEY_DYN_DATA,
                                     HKEY_LOCAL_MACHINE,
                                     HKEY_USERS};
static const wchar_t *possible_short_paths[] = {
    L"HKCR", L"HKCC", L"HKCU", L"HKDD", L"HKLM", L"HKU"};

static void test_main_format_arg_full_reg_path(void **state) {
    will_return_always(__wrap_RegOpenKeyEx, 9999999);

    expect_value_count(__wrap_export_single_value, format, OUTPUT_FORMAT_REG, -1);
    will_return_count(__wrap_export_single_value, true, 3);
    will_return_count(__wrap_export_single_value, false, 3);

    size_t full_reg_path_len = wcslen(FULL_REG_PATH_SUFFIX);
    wchar_t **buf = calloc(3, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(full_reg_path_len + 5, sizeof(wchar_t));
    buf[2] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);

    long unsigned int i;
    for (i = 0; i < ARRAY_SIZE(possible_hkey); i++) {

        expect_value(__wrap_export_single_value, top_key, possible_hkey[i]);
        wmemset(buf[1], L'\0', full_reg_path_len + 5);
        _snwprintf(buf[1],
                   full_reg_path_len + 4,
                   L"%ls%ls",
                   possible_short_paths[i],
                   FULL_REG_PATH_SUFFIX);
        expect_string(__wrap_export_single_value, reg_path, buf[1]);
        int ret = wmain(2, buf);
        assert_return_code(ret, i > 2 ? EXIT_FAILURE : EXIT_SUCCESS);
    }

    free(buf[0]);
    free(buf[1]);
    free(buf);
}

static void test_main_format_arg_reg(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(10, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-F", 2);

    expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_REG);
    wmemset(buf[2], L'\0', 10);
    wmemcpy(buf[2], L"reg", 3);
    int ret = wmain(3, buf);
    assert_return_code(ret, EXIT_SUCCESS);

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static void test_main_format_arg_default(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, true);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);

    expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_REG);
    int ret = wmain(1, buf);
    assert_return_code(ret, EXIT_SUCCESS);

    free(buf[0]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static void test_main_save_prefs_failed(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);
    will_return(__wrap_SHGetFolderPath, app_data_folder);
    will_return(__wrap_SHGetFolderPath, 0);
    will_return(__wrap_PathAppend, prefs_export_path);
    will_return(__wrap_PathAppend, true);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, false);

    will_return(__wrap_GetLastError, 0);
    will_return(__wrap_FormatMessage, 10);

    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);

    expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_REG);
    int ret = wmain(1, buf);
    assert_return_code(ret, EXIT_FAILURE);

    free(buf[0]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static const wchar_t *POWERSHELL_FORMAT_NAMES[] = {L"powershell", L"ps", L"ps1"};

static void test_main_format_arg_powershell(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, true);

    long unsigned int i;
    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(11, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 10);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-F", 2);
    for (i = 0; i < ARRAY_SIZE(POWERSHELL_FORMAT_NAMES); i++) {
        will_return(__wrap_SHGetFolderPath, app_data_folder);
        will_return(__wrap_SHGetFolderPath, 0);
        will_return(__wrap_PathAppend, prefs_export_path);
        will_return(__wrap_PathAppend, true);

        expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_POWERSHELL);
        wmemset(buf[2], L'\0', 11);
        wmemcpy(buf[2], POWERSHELL_FORMAT_NAMES[i], wcslen(POWERSHELL_FORMAT_NAMES[i]));
        int ret = wmain(3, buf);
        assert_return_code(ret, EXIT_SUCCESS);
    }

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

static const wchar_t *C_FORMAT_NAMES[] = {L"c", L"C"};

static void test_main_format_arg_c(void **state) {
    wchar_t *app_data_folder = calloc(30, sizeof(wchar_t));
    wmemset(app_data_folder, L'\0', 30);
    wmemcpy(app_data_folder, L"\\c\\users\\name\\AppData\\Roaming", 29);
    wchar_t *prefs_export_path = calloc(43, sizeof(wchar_t));
    wmemset(prefs_export_path, L'\0', 43);
    wmemcpy(prefs_export_path, app_data_folder, 29);
    wcsncat(prefs_export_path, L"prefs-export", 12);

    expect_any_always(__wrap_save_preferences, max_depth);
    expect_any_always(__wrap_save_preferences, commit);
    expect_any_always(__wrap_save_preferences, deploy_key);
    expect_any_always(__wrap_save_preferences, hk);
    expect_any_always(__wrap_save_preferences, output_dir);
    expect_any_always(__wrap_save_preferences, output_file);
    expect_any_always(__wrap_save_preferences, specified_path);
    will_return_always(__wrap_save_preferences, true);

    long unsigned int i;
    wchar_t **buf = calloc(4, sizeof(wchar_t *));
    buf[0] = calloc(8, sizeof(wchar_t));
    buf[1] = calloc(3, sizeof(wchar_t));
    buf[2] = calloc(7, sizeof(wchar_t));
    buf[3] = nullptr;
    wmemset(buf[0], L'\0', 8);
    wmemcpy(buf[0], L"winprefs", 7);
    wmemset(buf[1], L'\0', 3);
    wmemcpy(buf[1], L"-F", 2);
    for (i = 0; i < ARRAY_SIZE(C_FORMAT_NAMES); i++) {
        will_return(__wrap_SHGetFolderPath, app_data_folder);
        will_return(__wrap_SHGetFolderPath, 0);
        will_return(__wrap_PathAppend, prefs_export_path);
        will_return(__wrap_PathAppend, true);

        expect_value(__wrap_save_preferences, format, OUTPUT_FORMAT_C);
        wmemset(buf[2], L'\0', 7);
        wmemcpy(buf[2], C_FORMAT_NAMES[i], wcslen(C_FORMAT_NAMES[i]));
        int ret = wmain(3, buf);
        assert_return_code(ret, EXIT_SUCCESS);
    }

    free(buf[0]);
    free(buf[1]);
    free(buf[2]);
    free(buf);
    free(app_data_folder);
    free(prefs_export_path);
}

const struct CMUnitTest main_tests[] = {
    cmocka_unit_test(test_main_commit_arg),
    cmocka_unit_test(test_main_debug_arg),
    cmocka_unit_test(test_main_deploy_key_arg),
    cmocka_unit_test(test_main_format_arg_c),
    cmocka_unit_test(test_main_format_arg_c_sharp),
    cmocka_unit_test(test_main_format_arg_default),
    cmocka_unit_test(test_main_format_arg_full_reg_path),
    cmocka_unit_test(test_main_format_arg_invalid),
    cmocka_unit_test(test_main_format_arg_powershell),
    cmocka_unit_test(test_main_format_arg_reg),
    cmocka_unit_test(test_main_help),
    cmocka_unit_test(test_main_invalid_option),
    cmocka_unit_test(test_main_max_depth_conversion),
    cmocka_unit_test(test_main_output_dir_arg),
    cmocka_unit_test(test_main_output_file_arg),
    cmocka_unit_test(test_main_reg_path_invalid),
    cmocka_unit_test(test_main_reg_path_invalid_alt),
    cmocka_unit_test(test_main_save_prefs_failed),
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(main_tests, nullptr, nullptr);
}
