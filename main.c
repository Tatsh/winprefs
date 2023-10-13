#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

static const size_t MAX_KEY_LENGTH = 255;
static const size_t MAX_VALUE_NAME = 16383;
//static const size_t CMD_MAX_COMMAND_LENGTH = 8191;

char *escape_for_batch(const char *value) {
    char *s = NULL;
    if (value == NULL) {
        return s;
    }
    unsigned i, j;
    size_t len = strlen(value);
    size_t new_len = 0;
    for (i = 0; i < len; i++) {
        new_len++;
        if (value[i] == '\\' || value[i] == '%') {
            new_len++;
        }
    }
    s = malloc(MAX_KEY_LENGTH + 1);
    memset(s, 0, MAX_KEY_LENGTH);
    for (i = 0, j = 0; i < new_len && i < MAX_KEY_LENGTH; i++, j++) {
        s[j] = value[i];
        if (value[i] == '"' || value[i] == '%') {
            s[++j] = value[i];
        }
    }
    return s;
}

char *fix_v_param(const char *prop) {
    char *s;
    if (!strcmp("(default)", prop)) {
        s = malloc(5);
        memset(s, 0, 5);
        strncpy(s, "/ve ", 4);
    } else {
        char *escaped = escape_for_batch(prop);
        if (escaped == NULL) {
            return NULL;
        }
        size_t escaped_len = 7 + strlen(escaped);
        s = malloc(escaped_len);
        memset(s, 0, escaped_len);
        snprintf(s, escaped_len, "/v \"%s\" ", escaped);
        free(escaped);
    }
    return s;
}

char *convert_value_for_reg(DWORD reg_type, const unsigned char *data, size_t data_size) {
    if (reg_type == REG_NONE) {
        char *s = malloc(2);
        memset(s, 0, 2);
        strncpy(s, " ", 1);
        return s;
    } else if (reg_type == REG_BINARY) {
        // FIXME Incorrect size reported
        size_t i;
        char *bin_str = malloc(2 * data_size);
        memset(bin_str, 0, 2 * data_size);
        for (i = 0; i < 2 * data_size; i += 2) {
            char conv[3];
            snprintf(conv, 3, "%02x", data[i]);
            memcpy(bin_str + i, conv, 2);
        }
        size_t out_size = (2 * data_size) + 7;
        char *s = malloc(out_size);
        memset(s, 0, out_size);
        snprintf(s, out_size, " /d %s ", bin_str);
        return s;
    } else if (reg_type == REG_MULTI_SZ) {
    } else if (reg_type == REG_EXPAND_SZ || reg_type == REG_SZ) {
        char *s = escape_for_batch((char *)data);
        if (s == NULL) {
            return NULL;
        }
        size_t s_size = strlen(s) + 8;
        char *out = malloc(s_size);
        memset(out, 0, s_size);
        snprintf(out, s_size, " /d \"%s\" ", s);
        return out;
    } else if (reg_type == REG_DWORD || reg_type == REG_QWORD) {
        char *out = malloc(32);
        memset(out, 0, 32);
        if (reg_type == REG_DWORD) {
            snprintf(out, 32, " /d %lu ", *(DWORD *)data);
        } else {
            snprintf(out, 32, " /d %llu ", *(unsigned __int64 *)data);
        }
        return out;
    }
    return NULL;
}

void do_write_reg_command(HKEY h_key,
                          const char *stem,
                          const char *prior_stem,
                          const char *prop,
                          const unsigned char *value,
                          size_t value_len,
                          DWORD type) {
    (void)h_key;
    if (type == REG_NONE) {
        printf("Skipping value.\n");
        return;
    }
    char *escaped_d = convert_value_for_reg(type, value, value_len);
    char *path_after_hk = malloc(MAX_KEY_LENGTH);
    memset(path_after_hk, 0, MAX_KEY_LENGTH);
    if (prior_stem != NULL) {
        snprintf(path_after_hk, MAX_KEY_LENGTH, "%s", prior_stem);
    } else {
        memcpy(path_after_hk, stem, strlen(stem) + 1);
    }
    char *escaped_reg_key = escape_for_batch(prior_stem);
    char *reg_prop = fix_v_param(prop);
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

    printf("reg add \"%s\" %s/t %s%s/f\n",
           escaped_reg_key,
           reg_prop,
           reg_type,
           escaped_d ? escaped_d : "");

    if (escaped_d) {
        free(escaped_d);
    }
    if (reg_prop) {
        free(reg_prop);
    }
    if (escaped_reg_key) {
        free(escaped_reg_key);
    }
}

void do_write_reg_commands(HKEY h_key,
                           const char *stem,
                           unsigned n_values,
                           const char *prior_stem) {
    DWORD buf_len;
    DWORD i;
    DWORD type;
    DWORD value_len;
    char prop[MAX_VALUE_NAME];
    int ret = ERROR_SUCCESS;
    unsigned char buf[8192];
    for (i = 0; i < n_values; i++) {
        buf_len = sizeof(buf);
        prop[0] = '\0';
        value_len = MAX_VALUE_NAME;
        type = REG_NONE;
        ret = RegEnumValue(h_key, i, prop, &value_len, 0, &type, buf, &buf_len);
        if (ret == ERROR_MORE_DATA) {
            continue;
        }
        if (ret == ERROR_NO_MORE_ITEMS) {
            break;
        }
        do_write_reg_command(h_key, stem, prior_stem, prop, buf, value_len, type);
    }
}

void write_reg_commands(
    HKEY h_key, const char *stem, int max_depth, int *n_lines, int depth, const char *prior_stem) {
    if (depth >= max_depth) {
        fprintf(stderr, "Skipping %s due to depth limit of %d.\n", stem, max_depth);
        return;
    }
    // if (stem matches skipRE)
    HKEY h_key_out;
    char *full_path = NULL;
    full_path = malloc(MAX_KEY_LENGTH);
    memset(full_path, 0, MAX_KEY_LENGTH);
    snprintf(full_path, MAX_KEY_LENGTH, "%s", prior_stem);
    if (stem) {
        strncat(full_path, "\\", 1);
        strncat(full_path, stem, strlen(stem));
    }
    if (RegOpenKeyEx(h_key, stem, 0, KEY_READ, &h_key_out) == ERROR_SUCCESS) {
        DWORD n_sub_keys = 0;
        DWORD n_values = 0;
        LSTATUS ret_code = RegQueryInfoKey(h_key_out,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &n_sub_keys,
                                           NULL,
                                           NULL,
                                           &n_values,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL);
        if (n_sub_keys) {
            DWORD ach_key_len = 0;
            char ach_key[MAX_KEY_LENGTH];
            unsigned i;
            for (i = 0; i < n_sub_keys; i++) {
                ach_key_len = MAX_KEY_LENGTH;
                ret_code =
                    RegEnumKeyEx(h_key_out, i, ach_key, &ach_key_len, NULL, NULL, NULL, NULL);
                if (ret_code == ERROR_SUCCESS) {
                    int addend = 0;
                    write_reg_commands(
                        h_key_out, ach_key, max_depth, &addend, depth + 1, full_path);
                    n_lines += addend;
                } else {
                    fprintf(stderr, "Skipping %s because RegEnumKeyEx() failed.\n", full_path);
                }
            }
        } else {
            fprintf(stderr, "No subkeys at %s.\n", stem);
        }
        if (n_values) {
            do_write_reg_commands(h_key_out, stem, n_values, full_path);
        } else {
            fprintf(stderr, "No values at %s\n", stem);
        }
        RegCloseKey(h_key_out);
    } else {
        fprintf(stderr, "Skipping %s. Does the location exist?\n", stem);
    }
}

int save_preferences(
    bool commit, const char *deploy_key, const char *output_dir, int max_depth, HKEY h_key) {
    (void)commit;
    (void)deploy_key;
    (void)output_dir;
    // if deploy_key -> resolved_deploy_key = resolve_path(deploy_key)
    // mkdir(output_dir)
    int n_lines;
    write_reg_commands(h_key, NULL, max_depth, &n_lines, 0, "HKEY_CURRENT_USER");
    // if commit and has_git
    return 0;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    save_preferences(false, NULL, NULL, 20, HKEY_CURRENT_USER);
    return 0;
}
