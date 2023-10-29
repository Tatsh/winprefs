#include <wchar.h>

#include <shlwapi.h>
#include <windows.h>

#include "constants.h"
#include "debug.h"
#include "git.h"
#include "reg_command.h"
#include "registry.h"

HKEY get_top_key(wchar_t *reg_path) {
    if (!_wcsnicmp(reg_path, L"HKCR", 4) || !_wcsnicmp(reg_path, L"HKEY_CLASSES_ROOT", 17)) {
        return HKEY_CLASSES_ROOT;
    }
    if (!_wcsnicmp(reg_path, L"HKLM", 4) || !wcsnicmp(reg_path, L"HKEY_LOCAL_MACHINE", 18)) {
        return HKEY_LOCAL_MACHINE;
    }
    if (!_wcsnicmp(reg_path, L"HKCC", 4) || !_wcsnicmp(reg_path, L"HKEY_CURRENT_CONFIG", 19)) {
        return HKEY_CURRENT_CONFIG;
    }
    if (!_wcsnicmp(reg_path, L"HKU", 3) || !_wcsnicmp(reg_path, L"HKEY_USERS", 10)) {
        return HKEY_USERS;
    }
    if (!_wcsnicmp(reg_path, L"HKDD", 4) || !_wcsnicmp(reg_path, L"HKEY_DYN_DATA", 13)) {
        return HKEY_DYN_DATA;
    }
    return nullptr;
}

static inline bool create_dir_wrapper(wchar_t *folder) {
    if (!CreateDirectory(folder, NULL)) {
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
    while (end != NULL) {
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
                      const wchar_t *specified_path) {
    wchar_t full_output_dir[MAX_PATH];
    bool writing_to_stdout = !wcscmp(L"-", output_file);
    if (!_wfullpath(full_output_dir, output_dir, MAX_PATH)) {
        return false;
    }
    debug_print(L"Output directory: %ls\n", full_output_dir);
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
    write_reg_commands(hk,
                       nullptr,
                       max_depth,
                       0,
                       out_fp,
                       hk == HKEY_CLASSES_ROOT   ? L"HKCR" :
                       hk == HKEY_CURRENT_CONFIG ? L"HKCC" :
                       hk == HKEY_CURRENT_USER   ? L"HKCU" :
                       hk == HKEY_LOCAL_MACHINE  ? L"HKLM" :
                       hk == HKEY_USERS          ? L"HKU" :
                       hk == HKEY_DYN_DATA       ? L"HKDD" :
                                                   specified_path);
    if (!writing_to_stdout) {
        CloseHandle(out_fp);
    }
    if (commit && !writing_to_stdout) {
        git_commit(output_dir, deploy_key);
    }
    return true;
}

bool export_single_value(const wchar_t *reg_path, HKEY top_key) {
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
        starting_key, value_name, NULL, &reg_type, (LPBYTE)data, (LPDWORD)&buf_size);
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
    if (!do_write_reg_command(
            GetStdHandle(STD_OUTPUT_HANDLE), reg_path, value_name, data, buf_size, reg_type)) {
        return false;
    }
    free(data);
    return true;
}