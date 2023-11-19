#include "reg_command.h"
#include "constants.h"
#include "registry.h"
#include "shell.h"

static wchar_t *REG_PARAM_SLASH_VE = L"/ve ";

wchar_t *fix_v_param(const wchar_t *prop, size_t prop_len, bool *heap) {
    wchar_t *escaped, *out;
    escaped = out = nullptr;
    *heap = true;
    if (prop_len == 0 || prop == nullptr || !wcsncmp(L"(default)", prop, 9)) {
        *heap = false;
        out = REG_PARAM_SLASH_VE;
        goto cleanup;
    }
    escaped = escape_for_batch(prop, prop_len);
    size_t escaped_len = (7 + wcslen(escaped)) * WL;
    out = calloc(7 + wcslen(escaped), WL);
    if (!out) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    memset(out, 0, escaped_len);
    _snwprintf(out, escaped_len, L"/v \"%ls\" ", escaped);
    goto cleanup;
fail: // LCOV_EXCL_START
    out = nullptr;
    // LCOV_EXCL_STOP
cleanup:
    free(escaped);
    return out;
}

wchar_t *convert_data_for_reg(DWORD reg_type, const char *data, size_t data_len) {
    wchar_t *bin, *out, *s;
    bin = out = s = nullptr;
    if (reg_type == REG_BINARY) {
        size_t i;
        size_t n_bin_chars = 2 * data_len;
        size_t new_len = n_bin_chars + 1;
        bin = calloc(new_len, WL);
        if (!bin) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(bin, L'\0', new_len);
        for (i = 0; i < data_len; i++) {
            wchar_t conv[3];
            _snwprintf(conv, 3, L"%02x", data[i]);
            conv[2] = L'\0';
            wcsncat(&bin[i], conv, 2);
        }
        size_t s_size = new_len + 5;
        out = calloc(s_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L" /d %ls ", bin);
        goto cleanup;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ || reg_type == REG_MULTI_SZ) {
        size_t w_data_len = ((data_len % WL == 0) ? data_len / WL : (data_len / WL) + 1);
        wchar_t *w_data = (wchar_t *)data;
        size_t real_len = determine_multi_sz_size(w_data, w_data_len);
        if (real_len <= 2) {
            goto fail;
        }
        s = escape_for_batch((wchar_t *)data, real_len);
        if (!unlikely(s)) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        size_t s_size = (wcslen(s) + 8);
        out = calloc(s_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', s_size);
        _snwprintf(out, s_size, L" /d \"%ls\" ", s);
        goto cleanup;
    }
    if (reg_type == REG_DWORD || reg_type == REG_QWORD) {
        size_t s_size = 128;
        out = calloc(s_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        memset(out, 0, s_size);
        if (reg_type == REG_DWORD) {
            _snwprintf(out, s_size, L" /d %lu ", *(DWORD *)data);
        } else {
            _snwprintf(out, s_size, L" /d %llu ", *(UINT64 *)data);
        }
        goto cleanup;
    }
    errno = EINVAL;
fail:
    out = nullptr;
cleanup:
    free(bin);
    free(s);
    return out;
}

bool do_write_reg_command(HANDLE out_fp,
                          const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t data_len,
                          unsigned long type) {
    bool ret = true;
    wchar_t *out = nullptr;
    wchar_t *escaped_d = convert_data_for_reg(type, value, data_len);
    wchar_t *escaped_reg_key = escape_for_batch(full_path, wcslen(full_path));
    bool v_heap = false;
    wchar_t *v_param = fix_v_param(prop, prop ? wcslen(prop) : 0, &v_heap);
    wchar_t reg_type[14];
    if (!escaped_reg_key || !v_param) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    if (type == REG_MULTI_SZ && !escaped_d) {
        goto cleanup;
    }
    wmemset(reg_type, L'\0', 14);
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
    default:
        debug_print(L"Invalid registry data type.\n");
        goto fail;
    }
    int req_size = _snwprintf(nullptr,
                              0,
                              L"reg add \"%ls\" %ls/t %ls%ls/f",
                              escaped_reg_key,
                              v_param,
                              reg_type,
                              escaped_d ? escaped_d : L" ");
    if (req_size >= 1 && (size_t)req_size < CMD_MAX_COMMAND_LENGTH) {
        size_t total_size = (size_t)req_size + 1;
        out = calloc(total_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', total_size);
        int wrote = _snwprintf(out,
                               total_size,
                               L"reg add \"%ls\" %ls/t %ls%ls/f",
                               escaped_reg_key,
                               v_param,
                               reg_type,
                               escaped_d ? escaped_d : L" ");
        if (wrote) {
            ret = write_output(out_fp, out, true);
        }
    } else {
        errno = EKEYREJECTED;
        debug_print(L"%ls %ls: Skipping due to length of command.\n", full_path, prop);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free(escaped_d);
    if (v_heap) {
        free(v_param);
    }
    free(out);
    free(escaped_reg_key);
    return ret;
}
