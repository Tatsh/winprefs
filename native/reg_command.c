#include "reg_command.h"
#include "constants.h"
#include "registry.h"
#include "shell.h"

static wchar_t REG_PARAM_SLASH_VE[] = L"/ve ";
static wchar_t REG_PARAM_SLASH_V_EMPTY[] = L"/v \"\"";

wchar_t *fix_v_param(const wchar_t *prop, size_t prop_len, bool *heap) {
    wchar_t *escaped, *out;
    escaped = out = nullptr;
    *heap = true;
    if (!wcsncmp(L"(default)", prop, 9) || prop_len == 0 || prop == nullptr) {
        *heap = false;
        out = REG_PARAM_SLASH_VE;
        goto cleanup;
    }
    escaped = escape_for_batch(prop, prop_len);
    if (!escaped) {
        *heap = false;
        out = REG_PARAM_SLASH_V_EMPTY;
        goto cleanup;
    }
    size_t escaped_len = (7 + wcslen(escaped)) * WL;
    out = calloc(7 + wcslen(escaped), WL);
    if (!out) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    memset(out, 0, escaped_len);
    _snwprintf(out, escaped_len, L"/v \"%ls\" ", escaped);
    goto cleanup;
fail:
    out = nullptr;
cleanup:
    free_if_not_null(escaped);
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
        s = escape_for_batch((wchar_t *)data, data_len == 0 ? 0 : data_len / WL);
        if (!s) {
            goto fail;
        }
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
    free_if_not_null(bin);
    free_if_not_null(s);
    return out;
}

bool do_write_reg_command(HANDLE out_fp,
                          const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t data_len,
                          unsigned long type) {
    wchar_t *out = nullptr;
    wchar_t *escaped_d = convert_data_for_reg(type, value, data_len);
    wchar_t *escaped_reg_key = escape_for_batch(full_path, wcslen(full_path));
    bool v_heap = false;
    wchar_t *v_param = fix_v_param(prop, wcslen(prop), &v_heap);
    wchar_t reg_type[14];
    if (!escaped_reg_key || !v_param) {
        goto fail;
    }
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
    int req_size = _snwprintf(nullptr,
                              0,
                              L"reg add \"%ls\" %ls/t %ls%ls/f",
                              escaped_reg_key,
                              v_param,
                              reg_type,
                              escaped_d ? escaped_d : L" ");
    bool ret = true;
    if ((size_t)req_size < CMD_MAX_COMMAND_LENGTH) {
        size_t total_size = (size_t)req_size + 1;
        out = calloc(total_size, WL);
        if (!out) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(out, L'\0', total_size);
        int wrote = _snwprintf(out,
                               (size_t)req_size,
                               L"reg add \"%ls\" %ls/t %ls%ls/f",
                               escaped_reg_key,
                               v_param,
                               reg_type,
                               escaped_d ? escaped_d : L" ");

        if (((size_t)wrote < total_size) ||
            (wrote == req_size && out[total_size - 1] == L'f' && out[total_size - 2] == L'/' &&
             out[total_size - 3] == L' ')) {
            ret = write_output(out_fp, out, true);
        }
    } else {
        debug_print(L"%ls %ls: Skipping due to length of command.\n", full_path, prop);
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(escaped_d);
    if (v_heap) {
        free_if_not_null(v_param);
    }
    free_if_not_null(out);
    free_if_not_null(escaped_reg_key);
    return ret;
}
