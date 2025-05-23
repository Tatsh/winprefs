#include "powershell.h"
#include "constants.h"
#include "io.h"

static wchar_t *escape_for_powershell(const wchar_t *input, size_t n_chars) {
    wchar_t *out = nullptr;
    if (input == nullptr || n_chars == 0) { // LCOV_EXCL_START
        errno = EINVAL;
        return nullptr;
    } // LCOV_EXCL_STOP
    unsigned i, j;
    size_t new_n_chars = 0;
    for (i = 0; i < n_chars; i++) {
        new_n_chars++;
        // Last condition is to handle REG_MULTI_SZ string sets
        if (input[i] == L'\'') {
            new_n_chars++;
        }
    }
    out = calloc(new_n_chars + 1, WL);
    if (!out) { // LCOV_EXCL_START
        return nullptr;
    } // LCOV_EXCL_STOP
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
    wchar_t *out, *bin, *strings_nl, *escaped;
    out = bin = strings_nl = escaped = nullptr;
    if (reg_type == REG_BINARY) {
        size_t i;
        int j;
        size_t n_bin_chars = 5 * data_len;
        size_t new_len = n_bin_chars + 1;
        bin = calloc(new_len, WL);
        if (!bin) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
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
        out = calloc(s_size, WL);
        if (!out) { // LCOV_EXCL_START
            return nullptr;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L"(byte[]](%ls))", bin);
        goto cleanup;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ) {
        escaped = escape_for_powershell((wchar_t *)data, data_len == 0 ? 0 : data_len / WL);
        if (!escaped) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        size_t escaped_len = wcslen(escaped);
        size_t total_size = escaped_len + 3;
        out = calloc(total_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', total_size);
        _snwprintf(out, escaped_len + 2, L"'%ls'", escaped);
        goto cleanup;
    }
    if (reg_type == REG_MULTI_SZ) {
        wchar_t *w_data = (wchar_t *)data;
        size_t w_data_len = (data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1;
        size_t real_len = determine_multi_sz_size(w_data, w_data_len);
        if (real_len > 2) {
            size_t total_size = 9 + real_len;
            out = calloc(total_size, WL);
            if (!out) { // LCOV_EXCL_START
                goto fail;
            } // LCOV_EXCL_STOP
            wmemset(out, L'\0', total_size);
            strings_nl = calloc(w_data_len, WL);
            if (!strings_nl) { // LCOV_EXCL_START
                goto fail;
            } // LCOV_EXCL_STOP
            wmemset(strings_nl, L'\0', w_data_len);
            size_t i;
            for (i = 0; i < w_data_len - 2; i++) {
                strings_nl[i] = w_data[i] == L'\0' ? L'\n' : w_data[i];
            }
            _snwprintf(out, total_size, L"@\"\n%ls\n\"@", strings_nl);
            goto cleanup;
        }
        debug_print(L"Skipping incorrectly stored MultiString (length = %lu).\n", w_data_len);
        goto fail;
    }
    if (reg_type == REG_DWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%lu", *(DWORD *)data);
        out = calloc((size_t)(req_size + 1), WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)(req_size + 1));
        _snwprintf(out, (size_t)req_size, L"%lu", *(DWORD *)data);
        goto cleanup;
    }
    if (reg_type == REG_QWORD) {
        int req_size = _snwprintf(nullptr, 0, L"%llu", *(UINT64 *)data);
        out = calloc((size_t)(req_size + 1), WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, 0, (size_t)req_size);
        _snwprintf(out, (size_t)req_size, L"%llu", *(UINT64 *)data);
        goto cleanup;
    }
    errno = EINVAL;
fail:
    free(out);
cleanup:
    free(bin);
    free(escaped);
    free(strings_nl);
    return out;
}

static wchar_t *add_colon_if_required(const wchar_t *path) {
    wchar_t *escaped_reg_key, *full_path_ps, *top_key;
    escaped_reg_key = full_path_ps = top_key = nullptr;
    size_t full_path_len = wcslen(path) + 2;
    full_path_ps = calloc(full_path_len, WL);
    if (!full_path_ps) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(full_path_ps, L'\0', full_path_len);
    wchar_t *first_backslash = wcschr(path, '\\');
    if (!first_backslash) {
        goto fail;
    }
    size_t top_key_len = (size_t)((first_backslash - path) + 1);
    top_key = calloc(top_key_len, WL);
    if (!top_key) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(top_key, L'\0', top_key_len);
    wmemcpy(top_key, path, top_key_len - 1);
    _snwprintf(full_path_ps, full_path_len, L"%ls:%ls", top_key, first_backslash);
    full_path_ps[full_path_len - 1] = L'\0';
    escaped_reg_key = escape_for_powershell(full_path_ps, wcslen(full_path_ps));
    goto cleanup;
fail:
    escaped_reg_key = nullptr;
cleanup:
    free(full_path_ps);
    free(top_key);
    return escaped_reg_key;
}

bool do_write_powershell_reg_code(writer_t *writer,
                                  const wchar_t *full_path,
                                  const wchar_t *prop,
                                  const char *value,
                                  size_t data_len,
                                  unsigned long type) {
    wchar_t *out = nullptr;
    wchar_t *escaped_d = convert_data_for_powershell(type, value, data_len);
    wchar_t *escaped_reg_key = add_colon_if_required(full_path);
    wchar_t *escaped_prop = escape_for_powershell(prop, wcslen(prop));
    bool ret = true;
    if (!escaped_reg_key) {
        goto fail;
    }
    if (type == REG_MULTI_SZ && !escaped_d) {
        goto cleanup;
    }
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
    default: // LCOV_EXCL_START
        goto cleanup;
        // LCOV_EXCL_STOP
    }
    int req_size = _snwprintf(nullptr,
                              0,
                              POWERSHELL_CODE_TEMPLATE,
                              escaped_reg_key,
                              escaped_reg_key,
                              escaped_reg_key,
                              escaped_prop ? escaped_prop : L"(Default)",
                              reg_type,
                              escaped_d ? escaped_d : L"$null");
    if ((size_t)req_size < POWERSHELL_MAX_COMMAND_LENGTH) {
        out = calloc((size_t)req_size + 1, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', (size_t)req_size + 1);
        _snwprintf(out,
                   (size_t)req_size,
                   POWERSHELL_CODE_TEMPLATE,
                   escaped_reg_key,
                   escaped_reg_key,
                   escaped_reg_key,
                   escaped_prop ? escaped_prop : L"(Default)",
                   reg_type,
                   escaped_d ? escaped_d : L"$null");
        ret = write_output(out, false, writer);
    } else {
        debug_print(L"%ls %ls: Skipping due to length of command.\n", full_path, prop);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free(escaped_d);
    free(escaped_prop);
    free(escaped_reg_key);
    free(out);
    return ret;
}
