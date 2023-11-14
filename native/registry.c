#include "registry.h"
#include "constants.h"
#include "git.h"
#include "io.h"
#include "reg_code.h"
#include "reg_command.h"

static inline bool create_dir_wrapper(wchar_t *folder) {
    if (!CreateDirectory(folder, nullptr)) {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS && err != ERROR_ACCESS_DENIED) {
            return false;
        }
    }
    return true;
}

static inline bool create_dir_recursive(wchar_t *path) {
    wchar_t folder[MAX_PATH];
    wchar_t *end;
    wmemset(folder, L'\0', MAX_PATH);
    end = wcschr(path, L'\\');
    while (end != nullptr) {
        wcsncpy(folder, path, (size_t)(end - path + 1));
        if (!create_dir_wrapper(folder)) {
            return false;
        }
        end = wcschr(++end, L'\\');
    }
    if (!create_dir_wrapper(path)) {
        return false;
    }
    return true;
}

DLL_EXPORT bool save_preferences(bool commit,
                                 const wchar_t *deploy_key,
                                 const wchar_t *output_dir,
                                 const wchar_t *output_file,
                                 int max_depth,
                                 HKEY hk,
                                 const wchar_t *specified_path,
                                 enum OUTPUT_FORMAT format) {
    bool ret = true;
    wchar_t full_output_dir[MAX_PATH];
    bool writing_to_stdout = !wcscmp(L"-", output_file);
    if (!_wfullpath(full_output_dir, output_dir, MAX_PATH)) {
        goto fail;
    }
    if (!writing_to_stdout) {
        debug_print(L"Output directory: %ls\n", full_output_dir);
    } else {
        debug_print(L"Writing to standard output.\n");
    }
    if (!writing_to_stdout && !create_dir_recursive(full_output_dir)) {
        goto fail;
    }
    PathAppend(full_output_dir, output_file);
    full_output_dir[MAX_PATH - 1] = L'\0';
    HANDLE out_fp = !writing_to_stdout ? CreateFile(full_output_dir,
                                                    GENERIC_READ | GENERIC_WRITE,
                                                    0,
                                                    nullptr,
                                                    CREATE_ALWAYS,
                                                    FILE_ATTRIBUTE_NORMAL,
                                                    nullptr) :
                                         GetStdHandle(STD_OUTPUT_HANDLE);
    if (out_fp == INVALID_HANDLE_VALUE) {
        goto fail;
    }
    const wchar_t *prior_stem = hk == HKEY_CLASSES_ROOT   ? L"HKCR" :
                                hk == HKEY_CURRENT_CONFIG ? L"HKCC" :
                                hk == HKEY_CURRENT_USER   ? L"HKCU" :
                                hk == HKEY_LOCAL_MACHINE  ? L"HKLM" :
                                hk == HKEY_USERS          ? L"HKU" :
                                hk == HKEY_DYN_DATA       ? L"HKDD" :
                                                            specified_path;
    if (format == OUTPUT_FORMAT_C) {
        DWORD written;
        WriteFile(out_fp, C_PREAMBLE, (DWORD)SIZEOF_C_PREAMBLE, &written, nullptr);
    }
    ret = write_key_filtered_recursive(hk, nullptr, max_depth, 0, out_fp, prior_stem, format);
    if (!writing_to_stdout) {
        CloseHandle(out_fp);
    }
    if (ret && commit && !writing_to_stdout) {
        git_commit(output_dir, deploy_key);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    return ret;
}

DLL_EXPORT bool
export_single_value(HKEY top_key, const wchar_t *reg_path, enum OUTPUT_FORMAT format) {
    debug_print_enabled = true;
    bool ret = true;
    wchar_t *m_reg_path = nullptr;
    wchar_t *value_name = nullptr;
    char *data = nullptr;
    if (!reg_path) {
        errno = EINVAL;
        goto fail;
    }
    size_t reg_path_len = wcslen(reg_path) + 1;
    m_reg_path = calloc(reg_path_len, WL);
    wmemcpy(m_reg_path, reg_path, reg_path_len);
    wchar_t *first_backslash = wcschr(m_reg_path, L'\\');
    if (!first_backslash) {
        errno = EINVAL;
        goto fail;
    }
    wchar_t *subkey = first_backslash + 1;
    HKEY starting_key = HKEY_CURRENT_USER;
    wchar_t *last_backslash = wcsrchr(m_reg_path, '\\');
    wchar_t *value_name_p = last_backslash + 1;
    size_t value_name_len = wcslen(value_name_p) + 1;
    value_name = calloc(value_name_len, WL);
    if (!value_name) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(value_name, L'\0', value_name_len);
    wmemcpy(value_name, value_name_p, value_name_len);
    *last_backslash = L'\0';
    if (RegOpenKeyEx(top_key, subkey, 0, KEY_READ, &starting_key) != ERROR_SUCCESS) {
        debug_print(L"Invalid subkey: '%ls'.\n", subkey);
        goto fail;
    }
    size_t buf_size = 8192;
    data = malloc(buf_size);
    if (!data) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    DWORD reg_type = REG_NONE;
    LSTATUS reg_query_value_ret = RegQueryValueEx(
        starting_key, value_name, nullptr, &reg_type, (LPBYTE)data, (LPDWORD)&buf_size);
    if (reg_query_value_ret == ERROR_MORE_DATA) {
        debug_print(L"Value too large (%ls\\%ls).\n", subkey, value_name);
        goto fail;
    }
    if (reg_query_value_ret != ERROR_SUCCESS) {
        debug_print(L"Invalid value name '%ls'.\n", value_name);
        goto fail;
    }
    HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (format) {
    case OUTPUT_FORMAT_REG:
        if (!do_write_reg_command(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_C:
        if (!do_write_c_reg_code(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_C_SHARP:
        if (!do_write_c_sharp_reg_code(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    case OUTPUT_FORMAT_POWERSHELL:
        if (!do_write_powershell_reg_code(
                h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            goto fail;
        }
        break;
    default:
        errno = EINVAL;
        goto fail;
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(m_reg_path);
    free_if_not_null(value_name);
    free_if_not_null(data);
    return ret;
}
