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

bool save_preferences(bool commit,
                      const wchar_t *deploy_key,
                      const wchar_t *output_dir,
                      const wchar_t *output_file,
                      int max_depth,
                      HKEY hk,
                      const wchar_t *specified_path,
                      enum OUTPUT_FORMAT format) {
    wchar_t full_output_dir[MAX_PATH];
    bool writing_to_stdout = !wcscmp(L"-", output_file);
    if (!_wfullpath(full_output_dir, output_dir, MAX_PATH)) {
        return false;
    }
    if (!writing_to_stdout) {
        debug_print(L"Output directory: %ls\n", full_output_dir);
    } else {
        debug_print(L"Writing to standard output.\n");
    }
    if (!writing_to_stdout && !create_dir_recursive(full_output_dir)) {
        return false;
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
        return false;
    }
    bool ret = false;
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
    return ret;
}

bool export_single_value(const wchar_t *reg_path, HKEY top_key, enum OUTPUT_FORMAT format) {
    wchar_t *first_backslash = wcschr(reg_path, L'\\');
    if (!first_backslash) {
        return false;
    }
    wchar_t *subkey = first_backslash + 1;
    HKEY starting_key = HKEY_CURRENT_USER;
    wchar_t *last_backslash = wcsrchr(reg_path, '\\');
    wchar_t *value_name_p = last_backslash + 1;
    size_t value_name_len = wcslen(value_name_p);
    wchar_t *value_name = calloc(value_name_len, WL);
    if (!value_name) {
        return false;
    }
    wmemcpy(value_name, value_name_p, value_name_len);
    *last_backslash = L'\0';
    if (RegOpenKeyEx(top_key, subkey, 0, KEY_READ, &starting_key) != ERROR_SUCCESS) {
        free(value_name);
        debug_print(L"Invalid subkey: '%ls'.\n", subkey);
        return false;
    }
    size_t buf_size = 8192;
    char *data = malloc(buf_size);
    DWORD reg_type = REG_NONE;
    LSTATUS ret = RegQueryValueEx(
        starting_key, value_name, nullptr, &reg_type, (LPBYTE)data, (LPDWORD)&buf_size);
    if (ret == ERROR_MORE_DATA) {
        free(data);
        debug_print(L"Value too large (%ls\\%ls).\n", subkey, value_name);
        return false;
    }
    if (ret != ERROR_SUCCESS) {
        free(data);
        debug_print(L"Invalid value name '%ls'.\n", value_name);
        return false;
    }
    HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (format) {
    case OUTPUT_FORMAT_REG:
        if (!do_write_reg_command(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            return false;
        }
        break;
    case OUTPUT_FORMAT_C:
        if (!do_write_c_reg_code(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            return false;
        }
        break;
    case OUTPUT_FORMAT_C_SHARP:
        if (!do_write_c_sharp_reg_code(h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            return false;
        }
        break;
    case OUTPUT_FORMAT_POWERSHELL:
        if (!do_write_powershell_reg_code(
                h_stdout, reg_path, value_name, data, buf_size, reg_type)) {
            return false;
        }
        break;
    default:
        break;
    }
    free(data);
    return true;
}
