#include "io.h"
#include "constants.h"
#include "reg_code.h"
#include "reg_command.h"

bool write_output(HANDLE out_fp, wchar_t *out, bool use_crlf) {
    bool ret = true;
    char *mb_out = nullptr;
    DWORD written = 0;
    size_t req_size =
        (size_t)WideCharToMultiByte(CP_UTF8, 0, out, -1, nullptr, 0, nullptr, nullptr);
    if (req_size == 0) {
        goto cleanup;
    }
    size_t total_size = req_size + (use_crlf ? 1 : 0);
    mb_out = malloc(total_size);
    if (!mb_out) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    memset(mb_out, 0, total_size);
    if (!WideCharToMultiByte(CP_UTF8, 0, out, -1, mb_out, (int)total_size, nullptr, nullptr)) {
        free(mb_out);
        debug_print(L"Skipping possibly malformed wide character data.\n");
        return true;
    }
    if (use_crlf) {
        mb_out[total_size - 2] = '\r';
    }
    mb_out[total_size - 1] = '\n';
    ret = WriteFile(out_fp, mb_out, (DWORD)total_size, &written, nullptr);
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(mb_out);
    return ret && written > 0;
}

bool do_writes(HANDLE out_fp,
               HKEY hk,
               long unsigned n_values,
               const wchar_t *full_path,
               enum OUTPUT_FORMAT format) {
    bool ret = true;
    wchar_t *value = nullptr;
    if (n_values == 0) {
        goto cleanup;
    }
    if (!out_fp || !full_path) {
        errno = EINVAL;
        goto fail;
    }
    size_t data_len;
    DWORD i;
    DWORD reg_type;
    size_t value_len;
    value = calloc(MAX_VALUE_NAME, WL);
    if (!value) { // LCOV_EXCL_START
        return false;
    } // LCOV_EXCL_STOP
    if (format == OUTPUT_FORMAT_UNKNOWN) {
        goto fail;
    }
    LSTATUS enum_value_ret = ERROR_SUCCESS;
    char data[8192];
    for (i = 0; i < n_values; i++) {
        data_len = sizeof(data);
        wmemset(value, L'\0', MAX_VALUE_NAME);
        memset(data, 0, 8192);
        value_len = MAX_VALUE_NAME * WL;
        reg_type = REG_NONE;
        enum_value_ret = RegEnumValue(
            hk, i, value, (LPDWORD)&value_len, 0, &reg_type, (LPBYTE)data, (LPDWORD)&data_len);
        if (enum_value_ret == ERROR_MORE_DATA) { // LCOV_EXCL_START
            continue;
        }
        if (enum_value_ret == ERROR_NO_MORE_ITEMS) {
            break;
        }
        // LCOV_EXCL_STOP
        bool write_ret = false;
        switch (format) {
        case OUTPUT_FORMAT_REG:
            write_ret = do_write_reg_command(out_fp, full_path, value, data, data_len, reg_type);
            break;
        case OUTPUT_FORMAT_C:
            write_ret = do_write_c_reg_code(out_fp, full_path, value, data, data_len, reg_type);
            break;
        case OUTPUT_FORMAT_C_SHARP:
            write_ret =
                do_write_c_sharp_reg_code(out_fp, full_path, value, data, data_len, reg_type);
            break;
        case OUTPUT_FORMAT_POWERSHELL:
            write_ret =
                do_write_powershell_reg_code(out_fp, full_path, value, data, data_len, reg_type);
            break;
        default: // LCOV_EXCL_START
            goto fail;
            // LCOV_EXCL_STOP
        }
        if (!write_ret) {
            goto fail;
        }
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(value);
    return ret;
}

bool write_key_filtered_recursive(HKEY hk,
                                  const wchar_t *stem,
                                  int max_depth,
                                  int depth,
                                  HANDLE out_fp,
                                  const wchar_t *prior_stem,
                                  enum OUTPUT_FORMAT format) {
    bool ret = true;
    wchar_t *ach_key, *full_path;
    ach_key = full_path = nullptr;
    if (depth >= max_depth) {
        debug_print(L"%ls: Skipping %ls due to depth limit of %d.\n", prior_stem, stem, max_depth);
        errno = EDOM;
        goto cleanup;
    }
    HKEY hk_out;
    size_t full_path_len = WL * MAX_KEY_LENGTH;
    full_path = calloc(MAX_KEY_LENGTH, WL);
    if (!full_path) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(full_path, L'\0', MAX_KEY_LENGTH);
    size_t prior_stem_len = wcslen(prior_stem) * WL;
    size_t stem_len = stem ? wcslen(stem) : 0;
    if ((prior_stem_len + (stem_len * WL) + 2) > (full_path_len - 2)) {
        debug_print(L"%ls: Skipping %ls because of length limitation.\n", prior_stem, stem);
        errno = E2BIG;
        goto cleanup;
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
        errno = EKEYREJECTED;
        goto cleanup;
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
        if (ret_code != ERROR_SUCCESS) {
            goto fail;
        }
        if (n_sub_keys) {
            size_t ach_key_len = 0;
            ach_key = calloc(MAX_KEY_LENGTH, WL);
            if (!ach_key) { // LCOV_EXCL_START
                goto fail;
            } // LCOV_EXCL_STOP
            unsigned i;
            for (i = 0; i < n_sub_keys; i++) {
                ach_key_len = MAX_KEY_LENGTH;
                wmemset(ach_key, L'\0', MAX_KEY_LENGTH);
                ret_code = RegEnumKeyEx(
                    hk_out, i, ach_key, (LPDWORD)&ach_key_len, nullptr, nullptr, nullptr, nullptr);
                if (ret_code == ERROR_SUCCESS) {
                    if (!write_key_filtered_recursive(
                            hk_out, ach_key, max_depth, depth + 1, out_fp, full_path, format)) {
                        goto fail;
                    }
                } else {
                    debug_print(L"%ls: Skipping %ls because RegEnumKeyEx() failed.\n",
                                prior_stem,
                                full_path);
                }
            }
        } else {
            debug_print(L"%ls: No subkeys in %ls.\n", prior_stem, stem);
        }
        if (n_values) {
            if (!do_writes(out_fp, hk_out, n_values, full_path, format)) { // LCOV_EXCL_START
                goto fail;
            } // LCOV_EXCL_STOP
        } else {
            debug_print(L"%ls: No values in %ls.\n", prior_stem, stem);
        }
        RegCloseKey(hk_out);
    } else {
        debug_print(L"%ls: Skipping %ls. Does the location exist?\n", prior_stem, stem);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(ach_key);
    free_if_not_null(full_path);
    return ret;
}
