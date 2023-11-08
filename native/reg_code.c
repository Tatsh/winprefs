#include "reg_code.h"
#include "constants.h"
#include "reg_command.h"
#include "registry.h"

static wchar_t *get_top_key_string(const wchar_t *reg_path) {
    wchar_t *out = calloc(20, WL);
    if (!out) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
    wmemset(out, L'\0', 20);
    if (!_wcsnicmp(reg_path, L"HKCU", 4) || !_wcsnicmp(reg_path, L"HKEY_CURRENT_USER", 17)) {
        wmemcpy(out, L"HKEY_CURRENT_USER", 17);
    } else if (!_wcsnicmp(reg_path, L"HKCR", 4) || !_wcsnicmp(reg_path, L"HKEY_CLASSES_ROOT", 17)) {
        wmemcpy(out, L"HKEY_CLASSES_ROOT", 17);
    } else if (!_wcsnicmp(reg_path, L"HKLM", 4) ||
               !_wcsnicmp(reg_path, L"HKEY_LOCAL_MACHINE", 18)) {
        wmemcpy(out, L"HKEY_LOCAL_MACHINE", 18);
    } else if (!_wcsnicmp(reg_path, L"HKCC", 4) ||
               !_wcsnicmp(reg_path, L"HKEY_CURRENT_CONFIG", 19)) {
        wmemcpy(out, L"HKEY_CURRENT_CONFIG", 19);
    } else if (!_wcsnicmp(reg_path, L"HKU", 3) || !_wcsnicmp(reg_path, L"HKEY_USERS", 10)) {
        wmemcpy(out, L"HKEY_USERS", 10);
    } else if (!_wcsnicmp(reg_path, L"HKDD", 4) || !_wcsnicmp(reg_path, L"HKEY_DYN_DATA", 13)) {
        wmemcpy(out, L"HKEY_DYN_DATA", 13);
    } else {
        free(out);
        out = nullptr;
    }
    return out;
}

static wchar_t *escape_for_c(const wchar_t *input, size_t n_chars, bool escape_null) {
    if (input == nullptr || n_chars == 0) {
        errno = EINVAL;
        return nullptr;
    }
    unsigned i, j;
    size_t new_n_chars = 0;
    for (i = 0; i < n_chars; i++) {
        new_n_chars++;
        if (input[i] == L'"' || input[i] == L'\\' || (escape_null && input[i] == L'\0')) {
            new_n_chars++;
        }
    }
    wchar_t *out = calloc(new_n_chars + 1, WL);
    if (!out) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
    wmemset(out, L'\0', new_n_chars + 1);
    if (n_chars == new_n_chars) {
        wmemcpy(out, input, new_n_chars + 1);
        return out;
    }
    for (i = 0, j = 0; i < n_chars && j < new_n_chars; i++, j++) {
        if (input[i] == L'"' || input[i] == L'\\' || (escape_null && input[i] == L'\0')) {
            out[j++] = L'\\';
        }
        out[j] = (escape_null && input[i] == L'\0') ? L'0' : input[i];
    }
    return out;
}

static wchar_t *convert_data_for_c(DWORD reg_type, const char *data, size_t data_len) {
    if (reg_type == REG_BINARY) {
        size_t i;
        int j;
        size_t n_bin_chars = 7 * data_len;
        size_t new_len = n_bin_chars + 1;
        wchar_t *bin = calloc(new_len, WL);
        if (!bin) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(bin, L'\0', new_len);
        for (i = 0, j = 0; i < data_len; i++) {
            if (i < (data_len - 1)) {
                wchar_t conv[7];
                _snwprintf(conv, 6, L"0x%02x, ", data[i] & 0xFF);
                conv[6] = L'\0';
                wcsncat(bin + j, conv, 6);
                j += 6;
            } else {
                wchar_t conv[5];
                _snwprintf(conv, 4, L"0x%02x", data[i] & 0xFF);
                conv[4] = L'\0';
                wcsncat(bin + j, conv, 4);
                j += 4;
            }
        }
        size_t s_size = new_len + 4;
        wchar_t *out = calloc(s_size, WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L"{ %ls }", bin);
        free(bin);
        return out;
        return nullptr;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ) {
        size_t w_data_len = (data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1;
        wchar_t *escaped = escape_for_c((wchar_t *)data, data_len == 0 ? 0 : w_data_len, false);
        if (!escaped) {
            return nullptr;
        }
        size_t escaped_len = wcslen(escaped);
        size_t total_size = escaped_len + 1;
        wchar_t *out = calloc(total_size, WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', total_size);
        _snwprintf(out, escaped_len, L"%ls", escaped);
        return out;
    }
    if (reg_type == REG_MULTI_SZ) {
        wchar_t *w_data = (wchar_t *)data;
        size_t w_data_len = (data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1;
        if (w_data[w_data_len] == L'\0' && data[w_data_len - 1] == L'\0' && w_data_len > 2) {
            size_t total_size = 4 + w_data_len;
            wchar_t *escaped = escape_for_c((wchar_t *)data, w_data_len, true);
            if (!escaped) {
                return nullptr;
            }
            wchar_t *out = calloc(total_size, WL);
            if (!out) { // LCOV_EXCL_START
                return nullptr;
            } // LCOV_EXCL_STOP
            _snwprintf(out, total_size, L"%ls\\0", escaped);
            return out;
        }
        debug_print(L"Skipping incorrectly stored REG_MULTI_SZ (length = %d).\n", w_data_len);
        return nullptr;
    }
    if (reg_type == REG_DWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%lu", *(DWORD *)data);
        wchar_t *out = calloc((size_t)(req_size + 1), WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(out, 0, (size_t)req_size);
        _snwprintf(out, (size_t)req_size, L"%lu", *(DWORD *)data);
        return out;
    }
    if (reg_type == REG_QWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%llu", *(UINT64 *)data);
        wchar_t *out = calloc((size_t)(req_size + 1), WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(out, 0, (size_t)req_size);
        _snwprintf(out, (size_t)req_size, L"%llu", *(UINT64 *)data);
        return out;
    }
    errno = EINVAL;
    return nullptr;
}

bool do_write_c_reg_code(HANDLE out_fp,
                         const wchar_t *full_path,
                         const wchar_t *prop,
                         const char *value,
                         size_t data_len,
                         unsigned long type) {
    bool ret = true;
    wchar_t *subkey = wcschr(full_path, L'\\') + 1;
    wchar_t *escaped_key = escape_for_c(subkey, wcslen(subkey), false);
    wchar_t *escaped_d = convert_data_for_c(type, value, data_len);
    wchar_t *escaped_prop = escape_for_c(prop, wcslen(prop), false);
    wchar_t *top_key_s = get_top_key_string(full_path);

    if (!escaped_key || !escaped_d || !top_key_s) {
        return false;
    }

    wchar_t *out = nullptr;
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
    if (type == REG_DWORD || type == REG_QWORD) {
        int req_size = _snwprintf(nullptr,
                                  0,
                                  C_REGSETKEYVALUEW_TEMPLATE_NUMERIC,
                                  type == REG_DWORD ? L"d" : L"q",
                                  escaped_d,
                                  top_key_s,
                                  escaped_key,
                                  escaped_prop,
                                  reg_type,
                                  type == REG_DWORD ? KEYWORD_DWORD : KEYWORD_QWORD);
        out = calloc((size_t)req_size + 1, WL);
        if (!out) { // LCOV_EXCL_START
            return false;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   C_REGSETKEYVALUEW_TEMPLATE_NUMERIC,
                   type == REG_DWORD ? L"d" : L"q",
                   escaped_d,
                   top_key_s,
                   escaped_key,
                   escaped_prop,
                   reg_type,
                   type == REG_DWORD ? KEYWORD_DWORD : KEYWORD_QWORD);
    } else if (type == REG_SZ || type == REG_EXPAND_SZ || type == REG_MULTI_SZ) {
        int req_size = _snwprintf(nullptr,
                                  0,
                                  C_REGSETKEYVALUEW_TEMPLATE_SZ,
                                  top_key_s,
                                  escaped_key,
                                  escaped_prop,
                                  reg_type,
                                  escaped_d,
                                  data_len);
        out = calloc((size_t)req_size + 1, WL);
        if (!out) { // LCOV_EXCL_START
            return false;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   C_REGSETKEYVALUEW_TEMPLATE_SZ,
                   top_key_s,
                   escaped_key,
                   escaped_prop,
                   reg_type,
                   escaped_d,
                   data_len);
    } else if (type == REG_NONE) {
        int req_size = _snwprintf(
            nullptr, 0, C_REGSETKEYVALUEW_TEMPLATE_NONE, top_key_s, escaped_key, escaped_prop);
        out = calloc((size_t)req_size + 1, WL);
        if (!out) { // LCOV_EXCL_START
            return false;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   C_REGSETKEYVALUEW_TEMPLATE_NONE,
                   top_key_s,
                   escaped_key,
                   escaped_prop);
    } else if (type == REG_BINARY) {
        int req_size = _snwprintf(nullptr,
                                  0,
                                  C_REGSETKEYVALUEW_TEMPLATE_BINARY,
                                  escaped_d,
                                  top_key_s,
                                  escaped_key,
                                  escaped_prop,
                                  data_len);
        out = calloc((size_t)req_size + 1, WL);
        if (!out) { // LCOV_EXCL_START
            return false;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   C_REGSETKEYVALUEW_TEMPLATE_BINARY,
                   escaped_d,
                   top_key_s,
                   escaped_key,
                   escaped_prop,
                   data_len);
    } else {
        errno = EINVAL;
        ret = false;
    }
    if (ret && out) {
        ret = write_output(out_fp, out, false);
        free(out);
    }
    free(escaped_key);
    free(escaped_d);
    free(escaped_prop);
    if (top_key_s) {
        free(top_key_s);
    }
    return ret;
}

static wchar_t *convert_data_for_c_sharp(DWORD reg_type, const char *data, size_t data_len) {
    if (reg_type == REG_BINARY || reg_type == REG_NONE || reg_type == REG_QWORD ||
        reg_type == REG_DWORD) {
        return convert_data_for_c(reg_type, data, data_len);
    }
    if (reg_type == REG_SZ || reg_type == REG_EXPAND_SZ) {
        wchar_t *escaped = convert_data_for_c(reg_type, data, data_len);
        size_t escaped_len = wcslen(escaped);
        wchar_t *out = calloc(escaped_len + 3, WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        _snwprintf(out, escaped_len + 2, L"\"%ls\"", escaped);
        free(escaped);
        return out;
    }
    if (reg_type != REG_MULTI_SZ) {
        errno = EINVAL;
        return nullptr;
    }
    unsigned i, j;
    size_t strings_size = 0;
    wchar_t *w_data = (wchar_t *)data;
    for (i = 0; i < data_len; i++) {
        strings_size++;
        if (w_data[i] == L'"' || w_data[i] == L'\\') {
            strings_size++;
        } else if (w_data[i] == L'\0') {
            strings_size += 4;
        }
    }
    size_t total_size = 5 + strings_size;
    wchar_t *strings = calloc(strings_size + 1, WL);
    if (!strings) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
    wmemset(strings, L'\0', strings_size + 1);
    size_t w_data_len = (data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1;
    strings[0] = L'"';
    for (i = 0, j = 1; i < w_data_len - 2 && j < strings_size; i++, j++) {
        if (w_data[i] == L'\0') {
            strings[j++] = L'"';
            if (j < (strings_size - 1)) {
                strings[j++] = L',';
                strings[j++] = L' ';
                strings[j] = L'"';
            }
            continue;
        }
        if (w_data[i] == L'"' || w_data[i] == L'\\') {
            strings[j++] = L'\\';
        }
        strings[j] = w_data[i];
    }
    wchar_t *out = calloc(total_size, WL);
    if (!out) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
    _snwprintf(out, total_size - 1, L"{ %ls\" }", strings);
    return out;
}

bool do_write_c_sharp_reg_code(HANDLE out_fp,
                               const wchar_t *full_path,
                               const wchar_t *prop,
                               const char *value,
                               size_t data_len,
                               unsigned long type) {
    bool ret = true;
    wchar_t *subkey = wcschr(full_path, L'\\') + 1;
    wchar_t *escaped_key = escape_for_c(subkey, wcslen(subkey), false);
    wchar_t *escaped_d = convert_data_for_c_sharp(type, value, data_len);
    wchar_t *escaped_prop = escape_for_c(prop, wcslen(prop), false);
    wchar_t *top_key_s = get_top_key_string(full_path);
    wchar_t *out;
    wchar_t reg_type[33];
    memset(reg_type, 0, sizeof(reg_type));
    switch (type) {
    case REG_NONE:
        wcsncpy(reg_type, L", RegistryValueKind.None", 24);
        break;
    case REG_BINARY:
        wcsncpy(reg_type, L", RegistryValueKind.Binary", 26);
        break;
    case REG_EXPAND_SZ:
        wcsncpy(reg_type, L", RegistryValueKind.ExpandString", 32);
        break;
    case REG_SZ:
    case REG_MULTI_SZ:
        reg_type[0] = L'\0';
        break;
    case REG_DWORD:
        wcsncpy(reg_type, L", RegistryValueKind.DWord", 25);
        break;
    case REG_QWORD:
        wcsncpy(reg_type, L", RegistryValueKind.QWord", 25);
        break;
    }
    int req_size = _snwprintf(nullptr,
                              0,
                              C_SHARP_REGISTRY_SET_VALUE_TEMPLATE,
                              top_key_s,
                              escaped_key,
                              escaped_prop,
                              escaped_d,
                              reg_type);
    out = calloc((size_t)req_size + 1, WL);
    if (!out) { // LCOV_EXCL_START
        return false;
    } // LCOV_EXCL_STOP
    wmemset(out, L'\0', (size_t)req_size + 1);
    _snwprintf(out,
               (size_t)req_size,
               C_SHARP_REGISTRY_SET_VALUE_TEMPLATE,
               top_key_s,
               escaped_key,
               escaped_prop,
               escaped_d,
               reg_type);
    ret = write_output(out_fp, out, false);
    free(out);
    free(escaped_key);
    free(escaped_d);
    free(escaped_prop);
    if (top_key_s) {
        free(top_key_s);
    }
    return ret;
    return false;
}
