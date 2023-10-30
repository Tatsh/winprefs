#include <stdio.h>
#include <wchar.h>

#include <windows.h>
// Has to come after
#ifndef __WINE__
#include <versionhelpers.h>
#else
#define IsWindowsVistaOrGreater() 0
#endif

#include "constants.h"
#include "debug.h"
#include "macros.h"
#include "reg_command.h"
#include "shell.h"

static wchar_t REG_PARAM_SLASH_VE[] = L"/ve ";
static wchar_t REG_PARAM_SLASH_V_EMPTY[] = L"/v \"\"";

wchar_t *fix_v_param(const wchar_t *prop, size_t prop_len, bool *heap) {
    (void)prop_len;
    *heap = true;
    if (!wcsncmp(L"(default)", prop, 9) || prop_len == 0 || prop == nullptr) {
        *heap = false;
        return REG_PARAM_SLASH_VE;
    }
    wchar_t *escaped = escape_for_batch(prop, prop_len);
    if (escaped == nullptr) {
        *heap = false;
        return REG_PARAM_SLASH_V_EMPTY;
    }
    size_t escaped_len = (7 + wcslen(escaped)) * WL;
    wchar_t *out = calloc(7 + wcslen(escaped), WL);
    if (!out) {
        return nullptr;
    }
    memset(out, 0, escaped_len);
    _snwprintf(out, escaped_len, L"/v \"%ls\" ", escaped);
    free(escaped);
    return out;
}

wchar_t *convert_data_for_reg(DWORD reg_type, const char *data, size_t data_len) {
    if (reg_type == REG_BINARY) {
        size_t i;
        size_t n_bin_chars = 2 * data_len;
        size_t new_len = n_bin_chars + 1;
        wchar_t *bin = calloc(new_len, WL);
        if (!bin) {
            return nullptr;
        }
        wmemset(bin, L'\0', new_len);
        for (i = 0; i < data_len; i++) {
            wchar_t conv[3];
            _snwprintf(conv, 3, L"%02x", data[i]);
            conv[2] = L'\0';
            wcsncat(&bin[i], conv, 2);
        }
        size_t s_size = new_len + 5;
        wchar_t *out = calloc(s_size, WL);
        if (!out) {
            return nullptr;
        }
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L" /d %ls ", bin);
        free(bin);
        return out;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ || reg_type == REG_MULTI_SZ) {
        wchar_t *s = escape_for_batch((wchar_t *)data, data_len == 0 ? 0 : data_len / WL);
        if (s == nullptr) {
            return nullptr;
        }
        size_t s_size = (wcslen(s) + 8);
        wchar_t *out = calloc(s_size, WL);
        if (!out) {
            return nullptr;
        }
        memset(out, 0, s_size);
        _snwprintf(out, s_size, L" /d \"%ls\" ", s);
        free(s);
        return out;
    }
    if (reg_type == REG_DWORD || reg_type == REG_QWORD) {
        size_t s_size = 128;
        wchar_t *out = calloc(s_size, WL);
        if (!out) {
            return nullptr;
        }
        memset(out, 0, s_size);
        if (reg_type == REG_DWORD) {
            _snwprintf(out, s_size, L" /d %lu ", *(DWORD *)data);
        } else {
            _snwprintf(out, s_size, L" /d %llu ", *(unsigned __int64 *)data);
        }
        return out;
    }
    errno = EINVAL;
    return nullptr;
}

bool do_write_reg_command(HANDLE out_fp,
                          const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t data_len,
                          unsigned long type) {
    wchar_t *escaped_d = convert_data_for_reg(type, value, data_len);
    wchar_t *escaped_reg_key = escape_for_batch(full_path, wcslen(full_path));
    bool v_heap = false;
    wchar_t *v_param = fix_v_param(prop, wcslen(prop), &v_heap);
    wchar_t reg_type[14];
    memset(reg_type, 0, sizeof(reg_type));
    switch (type) {
    case REG_NONE:
        wcsncpy(reg_type, L"REG_NONE", 8);
        break;
    case REG_BINARY:
        wcsncpy(reg_type, L"REG_BINARY", 10);
        break;
    case REG_SZ:
        wcsncpy(reg_type, L"REG_SZ", 6);
        break;
    case REG_EXPAND_SZ:
        wcsncpy(reg_type, L"REG_EXPAND_SZ", 13);
        break;
    case REG_MULTI_SZ:
        wcsncpy(reg_type, L"REG_MULTI_SZ", 12);
        break;
    case REG_DWORD:
        wcsncpy(reg_type, L"REG_DWORD", 9);
        break;
    case REG_QWORD:
        wcsncpy(reg_type, L"REG_QWORD", 9);
        break;
    }
    wchar_t *out = calloc(CMD_MAX_COMMAND_LENGTH, WL);
    if (!out) {
        return false;
    }
    wmemset(out, L'\0', CMD_MAX_COMMAND_LENGTH);
    int wrote = _snwprintf(out,
                           CMD_MAX_COMMAND_LENGTH,
                           L"reg add \"%ls\" %ls/t %ls%ls/f",
                           escaped_reg_key,
                           v_param,
                           reg_type,
                           escaped_d ? escaped_d : L" ");
    if (((size_t)wrote < CMD_MAX_COMMAND_LENGTH) ||
        ((size_t)wrote == CMD_MAX_COMMAND_LENGTH && out[CMD_MAX_COMMAND_LENGTH - 1] == L'f' &&
         out[CMD_MAX_COMMAND_LENGTH - 2] == L'/' && out[CMD_MAX_COMMAND_LENGTH - 3] == L' ')) {
        size_t req_size =
            (size_t)WideCharToMultiByte(CP_UTF8,
                                        IsWindowsVistaOrGreater() ? _WC_ERR_INVALID_CHARS : 0,
                                        out,
                                        -1,
                                        nullptr,
                                        0,
                                        nullptr,
                                        nullptr);
        if (req_size == 0) {
            fwprintf(stdout, L"%ls\n", out);
        }
        char *mb_out = malloc(req_size + 3);
        if (!mb_out) {
            return false;
        }
        memset(mb_out, 0, req_size + 3);
        WideCharToMultiByte(CP_UTF8,
                            IsWindowsVistaOrGreater() ? _WC_ERR_INVALID_CHARS : 0,
                            out,
                            -1,
                            mb_out,
                            (int)req_size,
                            nullptr,
                            nullptr);
        mb_out[req_size - 1] = '\r';
        mb_out[req_size] = '\n';
        DWORD written = 0;
        bool ret = WriteFile(out_fp, mb_out, (DWORD)(req_size), &written, nullptr);
        free(mb_out);
        if (!ret || written == 0) {
            return false;
        }
    } else {
        debug_print(L"%ls %ls: Skipping due to length of command.\n", full_path, prop);
    }
    if (escaped_d) {
        free(escaped_d);
    }
    if (v_param && v_heap) {
        free(v_param);
    }
    free(out);
    free(escaped_reg_key);
    return true;
}

bool do_write_reg_commands(HANDLE out_fp, HKEY hk, unsigned n_values, const wchar_t *full_path) {
    if (n_values == 0) {
        return true;
    }
    if (!out_fp || !full_path) {
        errno = EINVAL;
        return false;
    }
    size_t data_len;
    DWORD i;
    DWORD reg_type;
    size_t value_len;
    wchar_t *value = calloc(MAX_VALUE_NAME, WL);
    if (!value) {
        return false;
    }
    int ret = ERROR_SUCCESS;
    char data[8192];
    for (i = 0; i < n_values; i++) {
        data_len = sizeof(data);
        wmemset(value, L'\0', MAX_VALUE_NAME);
        memset(data, 0, 8192);
        value_len = MAX_VALUE_NAME * WL;
        reg_type = REG_NONE;
        ret = RegEnumValue(
            hk, i, value, (LPDWORD)&value_len, 0, &reg_type, (LPBYTE)data, (LPDWORD)&data_len);
        if (ret == ERROR_MORE_DATA) {
            continue;
        }
        if (ret == ERROR_NO_MORE_ITEMS) {
            break;
        }
        do_write_reg_command(out_fp, full_path, value, data, data_len, reg_type);
    }
    free(value);
    return true;
}

bool write_reg_commands(HKEY hk,
                        const wchar_t *stem,
                        int max_depth,
                        int depth,
                        HANDLE out_fp,
                        const wchar_t *prior_stem) {
    if (depth >= max_depth) {
        debug_print(L"%ls: Skipping %ls due to depth limit of %d.\n", prior_stem, stem, max_depth);
        return true;
    }
    HKEY hk_out;
    size_t full_path_len = WL * MAX_KEY_LENGTH;
    wchar_t *full_path = calloc(MAX_KEY_LENGTH, WL);
    if (!full_path) {
        return false;
    }
    wmemset(full_path, L'\0', MAX_KEY_LENGTH);
    size_t prior_stem_len = wcslen(prior_stem) * WL;
    size_t stem_len = stem ? wcslen(stem) : 0;
    if ((prior_stem_len + (stem_len * WL) + 2) > (full_path_len - 2)) {
        debug_print(L"%ls: Skipping %ls because of length limitation.\n", prior_stem, stem);
        free(full_path);
        return true;
    }
    memcpy(full_path, prior_stem, prior_stem_len);
    if (stem) {
        wcsncat(full_path, L"\\", 1);
        wcsncat(full_path, stem, stem_len);
    }
    if (wcsstr(full_path, L"Classes\\Extensions\\ContractId\\Windows.BackgroundTasks\\PackageId") ||
        wcsstr(full_path, L"CloudStore\\Store\\Cache\\") ||
        wcsstr(full_path,
               L"CurrentVersion\\Authentication\\LogonUI\\Notifications\\BackgroundCapability") ||
        wcsstr(full_path, L"CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current\\") ||
        wcsstr(full_path, L"Explorer\\ComDlg32\\CIDSizeMRU") ||
        wcsstr(full_path, L"Explorer\\ComDlg32\\FirstFolder") ||
        wcsstr(full_path, L"Explorer\\ComDlg32\\LastVisitedPidlMRU") ||
        wcsstr(full_path, L"Explorer\\ComDlg32\\OpenSavePidlMRU") ||
        wcsstr(full_path, L"IrisService\\Cache") ||
        wcsstr(full_path, L"Microsoft\\Windows\\Shell\\Bags") ||
        wcsstr(full_path, L"Windows\\Shell\\BagMRU") ||
        wcsstr(full_path, L"Windows\\Shell\\MuiCache")) {
        debug_print(L"%ls: Skipping %ls due to filter.\n", prior_stem, stem);
        free(full_path);
        return true;
    }
    if (RegOpenKeyEx(hk, stem, 0, KEY_READ, &hk_out) == ERROR_SUCCESS) {
        DWORD n_sub_keys = 0;
        DWORD n_values = 0;
        LSTATUS ret_code = RegQueryInfoKey(hk_out,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           &n_sub_keys,
                                           nullptr,
                                           nullptr,
                                           &n_values,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           nullptr);
        if (n_sub_keys) {
            size_t ach_key_len = 0;
            wchar_t *ach_key = calloc(MAX_KEY_LENGTH, WL);
            if (!ach_key) {
                return false;
            }
            unsigned i;
            for (i = 0; i < n_sub_keys; i++) {
                ach_key_len = MAX_KEY_LENGTH;
                wmemset(ach_key, L'\0', MAX_KEY_LENGTH);
                ret_code = RegEnumKeyEx(
                    hk_out, i, ach_key, (LPDWORD)&ach_key_len, nullptr, nullptr, nullptr, nullptr);
                if (ret_code == ERROR_SUCCESS) {
                    write_reg_commands(hk_out, ach_key, max_depth, depth + 1, out_fp, full_path);
                } else {
                    debug_print(L"%ls: Skipping %ls because RegEnumKeyEx() failed.\n",
                                prior_stem,
                                full_path);
                }
            }
            free(ach_key);
        } else {
            debug_print(L"%ls: No subkeys in %ls.\n", prior_stem, stem);
        }
        if (n_values) {
            if (!do_write_reg_commands(out_fp, hk_out, n_values, full_path)) {
                return false;
            }
        } else {
            debug_print(L"%ls: No values in %ls.\n", prior_stem, stem);
        }
        RegCloseKey(hk_out);
    } else {
        debug_print(L"%ls: Skipping %ls. Does the location exist?\n", prior_stem, stem);
    }
    free(full_path);
    return true;
}
