#include <wchar.h>

#include "constants.h"
#include "debug.h"
#include "io.h"
#include "powershell.h"

static wchar_t *escape_for_powershell(const wchar_t *input, size_t n_chars) {
    if (input == nullptr || n_chars == 0) {
        errno = EINVAL;
        return nullptr;
    }
    unsigned i, j;
    size_t new_n_chars = 0;
    for (i = 0; i < n_chars; i++) {
        new_n_chars++;
        // Last condition is to handle REG_MULTI_SZ string sets
        if (input[i] == L'\'') {
            new_n_chars++;
        }
    }
    wchar_t *out = calloc(new_n_chars + 1, WL);
    if (!out) {
        return nullptr;
    }
    wmemset(out, L'\0', new_n_chars + 1);
    if (n_chars == new_n_chars) {
        wmemcpy(out, input, new_n_chars + 1);
        return out;
    }
    for (i = 0, j = 0; i < n_chars && j < new_n_chars; i++, j++) {
        out[j] = input[i];
        if (input[i] == L'\'') {
            out[++j] = input[i];
        }
    }
    return out;
}

static wchar_t *convert_data_for_powershell(DWORD reg_type, const char *data, size_t data_len) {
    if (reg_type == REG_BINARY) {
        size_t i;
        int j;
        size_t n_bin_chars = 5 * data_len;
        size_t new_len = n_bin_chars + 1;
        wchar_t *bin = calloc(new_len, WL);
        if (!bin) {
            return nullptr;
        }
        wmemset(bin, L'\0', new_len);
        for (i = 0, j = 0; i < data_len; i++) {
            if (i < (data_len - 1)) {
                wchar_t conv[6];
                _snwprintf(conv, 6, L"0x%02X,", data[i] & 0xFF);
                conv[5] = L'\0';
                wcsncat(bin + j, conv, 5);
                j += 5;
            } else {
                wchar_t conv[5];
                _snwprintf(conv, 4, L"0x%02X", data[i] & 0xFF);
                conv[4] = L'\0';
                wcsncat(bin + j, conv, 4);
                j += 4;
            }
        }
        size_t s_size = new_len + 10;
        wchar_t *out = calloc(s_size, WL);
        if (!out) {
            return nullptr;
        }
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L"(byte[]](%ls))", bin);
        free(bin);
        return out;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ) {
        wchar_t *escaped =
            escape_for_powershell((wchar_t *)data, data_len == 0 ? 0 : data_len / WL);
        size_t escaped_len = wcslen(escaped);
        size_t total_size = escaped_len + 3;
        wchar_t *out = calloc(total_size, WL);
        wmemset(out, L'\0', total_size);
        _snwprintf(out, escaped_len + 2, L"'%ls'", escaped);
        return out;
    }
    if (reg_type == REG_MULTI_SZ) {
        wchar_t *w_data = (wchar_t *)data;
        size_t w_data_len = (data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1;
        if (w_data[w_data_len] == L'\0' && data[w_data_len - 1] == L'\0' && w_data_len > 2) {
            size_t total_size = 9 + w_data_len;
            wchar_t *out = calloc(total_size, WL);
            wmemset(out, L'\0', total_size);
            wchar_t *strings_nl = calloc(w_data_len, WL);
            wmemset(strings_nl, L'\0', w_data_len);
            size_t i;
            for (i = 0; i < w_data_len - 2; i++) {
                strings_nl[i] = w_data[i] == L'\0' ? L'\n' : w_data[i];
            }
            _snwprintf(out, total_size, L"@\"\n%ls\n\"@", strings_nl);
            return out;
        }
        debug_print(L"Skipping incorrectly stored MultiString (length = %d).\n", w_data_len);
        return nullptr;
    }
    if (reg_type == REG_DWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%lu", *(DWORD *)data);
        wchar_t *out = calloc((size_t)(req_size + 1), WL);
        if (!out) {
            return nullptr;
        }
        wmemset(out, 0, (size_t)req_size);
        _snwprintf(out, (size_t)req_size, L"%lu", *(DWORD *)data);
        return out;
    }
    if (reg_type == REG_QWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%llu", *(unsigned __int64 *)data);
        wchar_t *out = calloc((size_t)(req_size + 1), WL);
        if (!out) {
            return nullptr;
        }
        wmemset(out, 0, (size_t)req_size);
        _snwprintf(out, (size_t)req_size, L"%llu", *(unsigned __int64 *)data);
        return out;
    }
    errno = EINVAL;
    return nullptr;
}

static wchar_t *add_colon_if_required(const wchar_t *path) {
    size_t full_path_len = wcslen(path) + 2;
    wchar_t *full_path_ps = calloc(full_path_len, WL);
    wmemset(full_path_ps, L'\0', full_path_len);
    wchar_t *first_backslash = wcschr(path, '\\');
    size_t top_key_len = (size_t)((first_backslash - path) + 1);
    wchar_t *top_key = calloc(top_key_len, WL);
    wmemset(top_key, L'\0', top_key_len);
    wmemcpy(top_key, path, top_key_len - 1);
    _snwprintf(full_path_ps, full_path_len - 1, L"%ls:%ls", top_key, first_backslash);
    wchar_t *escaped_reg_key = escape_for_powershell(full_path_ps, wcslen(full_path_ps));
    free(top_key);
    free(full_path_ps);
    return escaped_reg_key;
}

bool do_write_powershell_reg_code(HANDLE out_fp,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type) {
    wchar_t *escaped_d = convert_data_for_powershell(type, value, data_len);
    wchar_t *escaped_reg_key = add_colon_if_required(full_path);
    wchar_t *escaped_prop = escape_for_powershell(prop, wcslen(prop));
    wchar_t reg_type[14];
    memset(reg_type, 0, sizeof(reg_type));
    switch (type) {
    case REG_NONE:
        wcsncpy(reg_type, L"None", 4);
        break;
    case REG_BINARY:
        wcsncpy(reg_type, L"Binary", 6);
        break;
    case REG_SZ:
        wcsncpy(reg_type, L"String", 6);
        break;
    case REG_EXPAND_SZ:
        wcsncpy(reg_type, L"ExpandString", 12);
        break;
    case REG_MULTI_SZ:
        wcsncpy(reg_type, L"MultiString", 11);
        break;
    case REG_DWORD:
        wcsncpy(reg_type, L"DWord", 5);
        break;
    case REG_QWORD:
        wcsncpy(reg_type, L"Qword", 5);
        break;
    }
    int req_size = _snwprintf(nullptr,
                              0,
                              POWERSHELL_CODE_TEMPLATE,
                              escaped_reg_key,
                              escaped_reg_key,
                              escaped_reg_key,
                              escaped_prop,
                              reg_type,
                              escaped_d);
    bool ret = true;
    if ((size_t)req_size < POWERSHELL_MAX_COMMAND_LENGTH) {
        wchar_t *out = calloc((size_t)req_size + 1, WL);
        if (!out) {
            return false;
        }
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   POWERSHELL_CODE_TEMPLATE,
                   escaped_reg_key,
                   escaped_reg_key,
                   escaped_reg_key,
                   escaped_prop,
                   reg_type,
                   escaped_d);
        ret = write_output(out_fp, out, false);
        free(out);
    } else {
        debug_print(L"%ls %ls: Skipping due to length of command.\n", full_path, prop);
    }
    free(escaped_prop);
    free(escaped_d);
    free(escaped_reg_key);
    return ret;
}
