#include <stdio.h>

#include "constants.h"
#include "reg_command.h"
#include "shell.h"

static char REG_PARAM_SLASH_VE[] = "/ve ";

char *fix_v_param(const char *prop, size_t prop_len, bool *heap) {
    *heap = true;
    if (!strncmp("(default)", prop, 9)) {
        *heap = false;
        return REG_PARAM_SLASH_VE;
    }
    char *escaped = escape_for_batch(prop, prop_len);
    if (escaped == nullptr) {
        return nullptr;
    }
    size_t escaped_len = 7 + strlen(escaped);
    char *out = malloc(escaped_len);
    if (!out) {
        abort();
    }
    memset(out, 0, escaped_len);
    snprintf(out, escaped_len, "/v \"%s\" ", escaped);
    free(escaped);
    return out;
}

char *convert_data_for_reg(DWORD reg_type, const unsigned char *data, size_t data_len) {
    if (reg_type == REG_BINARY) {
        size_t i;
        size_t new_len = (2 * data_len) + 1;
        char *bin = malloc(new_len);
        if (!bin) {
            abort();
        }
        memset(bin, 0, new_len);
        for (i = 0; i < (new_len - 1); i += 2) {
            char conv[3];
            snprintf(conv, 3, "%02x", data[i]);
            memcpy(bin + i, conv, 2);
        }
        size_t out_size = (2 * data_len) + 7;
        char *s = malloc(out_size);
        if (!s) {
            abort();
        }
        memset(s, 0, out_size);
        snprintf(s, out_size, " /d %s ", bin);
        return s;
    }
    if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ || reg_type == REG_MULTI_SZ) {
        char *s = escape_for_batch((char *)data, data_len);
        if (s == nullptr) {
            return nullptr;
        }
        size_t s_size = strlen(s) + 8;
        char *out = malloc(s_size);
        if (!out) {
            abort();
        }
        memset(out, 0, s_size);
        snprintf(out, s_size, " /d \"%s\" ", s);
        return out;
    }
    if (reg_type == REG_DWORD || reg_type == REG_QWORD) {
        char *out = malloc(32);
        if (!out) {
            abort();
        }
        memset(out, 0, 32);
        if (reg_type == REG_DWORD) {
            snprintf(out, 32, " /d %lu ", *(DWORD *)data);
        } else {
            snprintf(out, 32, " /d %llu ", *(unsigned __int64 *)data);
        }
        return out;
    }
    return nullptr;
}

void do_write_reg_command(HKEY hk,
                          const char *full_path,
                          const char *prop,
                          const unsigned char *value,
                          size_t data_len,
                          DWORD type,
                          bool debug) {
    (void)hk;
    char *escaped_d = convert_data_for_reg(type, value, data_len);
    char *escaped_reg_key = escape_for_batch(full_path, strlen(full_path));
    bool v_heap = false;
    char *v_param = fix_v_param(prop, strlen(prop), &v_heap);
    char reg_type[32] = "REG_NONE";
    if (type != REG_NONE) {
        memset(reg_type, 0, sizeof(reg_type));
        switch (type) {
        case REG_BINARY:
            strncpy(reg_type, "REG_BINARY", 10);
            break;
        case REG_SZ:
            strncpy(reg_type, "REG_SZ", 6);
            break;
        case REG_EXPAND_SZ:
            strncpy(reg_type, "REG_EXPAND_SZ", 13);
            break;
        case REG_MULTI_SZ:
            strncpy(reg_type, "REG_MULTI_SZ", 12);
            break;
        case REG_DWORD:
            strncpy(reg_type, "REG_DWORD", 9);
            break;
        case REG_QWORD:
            strncpy(reg_type, "REG_QWORD", 9);
            break;
        }
    }
    char out[CMD_MAX_COMMAND_LENGTH];
    int wrote = snprintf(out,
                         CMD_MAX_COMMAND_LENGTH,
                         "reg add \"%s\" %s/t %s%s/f",
                         escaped_reg_key,
                         v_param,
                         reg_type,
                         escaped_d ? escaped_d : " ");
    if (((size_t)wrote < CMD_MAX_COMMAND_LENGTH) ||
        ((size_t)wrote == CMD_MAX_COMMAND_LENGTH && out[CMD_MAX_COMMAND_LENGTH - 1] == 'f' &&
         out[CMD_MAX_COMMAND_LENGTH - 2] == '/' && out[CMD_MAX_COMMAND_LENGTH - 3] == ' ')) {
        printf("%s\n", out);
    } else {
        if (debug) {
            fprintf(stderr, "%s %s: Skipping due to length of command.", full_path, prop);
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

void do_write_reg_commands(HKEY hk, unsigned n_values, const char *full_path, bool debug) {
    DWORD data_len;
    DWORD i;
    DWORD reg_type;
    DWORD value_len;
    char value[MAX_VALUE_NAME];
    int ret = ERROR_SUCCESS;
    unsigned char data[8192];
    for (i = 0; i < n_values; i++) {
        data_len = sizeof(data);
        value[0] = '\0';
        value_len = MAX_VALUE_NAME;
        reg_type = REG_NONE;
        ret = RegEnumValue(hk, i, value, &value_len, 0, &reg_type, data, &data_len);
        if (ret == ERROR_MORE_DATA) {
            continue;
        }
        if (ret == ERROR_NO_MORE_ITEMS) {
            break;
        }
        do_write_reg_command(hk, full_path, value, data, data_len, reg_type, debug);
    }
}
