#include <stdio.h>

#include "constants.h"
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
    wchar_t *out = malloc(escaped_len);
    if (!out) {
        abort();
    }
    memset(out, 0, escaped_len);
    _snwprintf(out, escaped_len, L"/v \"%s\" ", escaped);
    free(escaped);
    return out;
}

wchar_t *convert_data_for_reg(DWORD reg_type, const char *data, size_t data_len) {
    if (reg_type == REG_BINARY) {
        size_t i;
        size_t n_bin_chars = 2 * data_len;
        size_t new_len = (n_bin_chars + 1) * WL;
        wchar_t *bin = malloc(new_len);
        if (!bin) {
            abort();
        }
        memset(bin, 0, new_len);
        for (i = 0; i < data_len; i++) {
            wchar_t conv[3];
            _snwprintf(conv, 3, L"%02x", data[i]);
            wcsncat(&bin[i], conv, 2);
        }
        size_t s_size = new_len + (5 * WL);
        wchar_t *out = malloc(s_size);
        if (!out) {
            abort();
        }
        memset(out, 0, s_size);
        _snwprintf(out, s_size, L" /d %ls ", bin);
        return out;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ || reg_type == REG_MULTI_SZ) {
        wchar_t *s = escape_for_batch((wchar_t *)data, data_len == 0 ? 0 : (data_len / WL) - 1);
        if (s == nullptr) {
            return nullptr;
        }
        size_t s_size = (wcslen(s) + 8) * WL;
        wchar_t *out = malloc(s_size);
        if (!out) {
            abort();
        }
        memset(out, 0, s_size);
        _snwprintf(out, s_size, L" /d \"%s\" ", s);
        return out;
    }
    if (reg_type == REG_DWORD || reg_type == REG_QWORD) {
        size_t s_size = 32 * WL;
        wchar_t *out = malloc(s_size);
        if (!out) {
            abort();
        }
        memset(out, 0, s_size);
        if (reg_type == REG_DWORD) {
            _snwprintf(out, s_size, L" /d %lu ", *(DWORD *)data);
        } else {
            _snwprintf(out, s_size, L" /d %llu ", *(unsigned __int64 *)data);
        }
        return out;
    }
    return nullptr;
}

void do_write_reg_command(const wchar_t *full_path,
                          const wchar_t *prop,
                          const char *value,
                          size_t data_len,
                          DWORD type,
                          bool debug) {
    (void)value;
    (void)data_len;
    wchar_t *escaped_d = convert_data_for_reg(type, value, data_len);
    wchar_t *escaped_reg_key = escape_for_batch(full_path, wcslen(full_path));
    bool v_heap = false;
    wchar_t *v_param = fix_v_param(prop, wcslen(prop), &v_heap);
    wchar_t reg_type[13];
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
    wchar_t out[CMD_MAX_COMMAND_LENGTH];
    int wrote = _snwprintf(out,
                           CMD_MAX_COMMAND_LENGTH,
                           L"reg add \"%ls\" %ls/t %s%ls/f",
                           escaped_reg_key,
                           v_param,
                           reg_type,
                           escaped_d ? escaped_d : L" ");
    if (((size_t)wrote < CMD_MAX_COMMAND_LENGTH) ||
        ((size_t)wrote == CMD_MAX_COMMAND_LENGTH && out[CMD_MAX_COMMAND_LENGTH - 1] == L'f' &&
         out[CMD_MAX_COMMAND_LENGTH - 2] == L'/' && out[CMD_MAX_COMMAND_LENGTH - 3] == L' ')) {
        wprintf(L"%ls\n", out);
    } else {
        if (debug) {
            fwprintf(stderr, L"%ls %ls: Skipping due to length of command.", full_path, prop);
        }
    }
    if (escaped_d) {
        free(escaped_d);
    }
    if (v_param && v_heap) {
        free(v_param);
    }
    free(escaped_reg_key);
}

void do_write_reg_commands(HKEY hk, unsigned n_values, const wchar_t *full_path, bool debug) {
    DWORD data_len;
    DWORD i;
    DWORD reg_type;
    DWORD value_len;
    wchar_t value[MAX_VALUE_NAME];
    int ret = ERROR_SUCCESS;
    char data[8192];
    for (i = 0; i < n_values; i++) {
        data_len = sizeof(data);
        value[0] = '\0';
        value_len = MAX_VALUE_NAME * WL;
        reg_type = REG_NONE;
        ret = RegEnumValue(hk, i, value, &value_len, 0, &reg_type, (LPBYTE)data, &data_len);
        if (ret == ERROR_MORE_DATA) {
            continue;
        }
        if (ret == ERROR_NO_MORE_ITEMS) {
            break;
        }
        do_write_reg_command(full_path, value, data, data_len, reg_type, debug);
    }
}
