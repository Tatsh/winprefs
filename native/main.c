#include "unicode.h"

#include <stdio.h>

#include <windows.h>

#include "constants.h"
#include "reg_command.h"

void write_reg_commands(
    HKEY hk, const char *stem, int max_depth, int depth, const char *prior_stem, bool debug) {
    if (depth >= max_depth) {
        if (debug) {
            fprintf(
                stderr, "%s: Skipping %s due to depth limit of %d.\n", prior_stem, stem, max_depth);
        }
        return;
    }
    HKEY hk_out;
    char *full_path = malloc(MAX_KEY_LENGTH);
    if (full_path == nullptr) {
        fprintf(stderr, "%s: Stopping due to malloc error.", prior_stem);
        abort();
    }
    memset(full_path, 0, MAX_KEY_LENGTH);
    size_t prior_stem_len = strlen(prior_stem);
    size_t stem_len = stem ? strlen(stem) : 0;
    if ((prior_stem_len + stem_len + 1) > (MAX_KEY_LENGTH - 1)) {
        if (debug) {
            fprintf(stderr, "%s: Skipping %s because of length limitation", prior_stem, stem);
        }
        free(full_path);
        return;
    }
    memcpy(full_path, prior_stem, prior_stem_len);
    if (stem) {
        strncat(full_path, "\\", 1);
        strncat(full_path, stem, stem_len);
    }
    if (strstr(full_path, "Classes\\Extensions\\ContractId\\Windows.BackgroundTasks\\PackageId") ||
        strstr(full_path, "CloudStore\\Store\\Cache\\") ||
        strstr(full_path,
               "CurrentVersion\\Authentication\\LogonUI\\Notifications\\BackgroundCapability") ||
        strstr(full_path, "CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current\\") ||
        strstr(full_path, "Explorer\\ComDlg32\\CIDSizeMRU") ||
        strstr(full_path, "Explorer\\ComDlg32\\FirstFolder") ||
        strstr(full_path, "Explorer\\ComDlg32\\LastVisitedPidlMRU") ||
        strstr(full_path, "Explorer\\ComDlg32\\OpenSavePidlMRU") ||
        strstr(full_path, "IrisService\\Cache") ||
        strstr(full_path, "Microsoft\\Windows\\Shell\\Bags") ||
        strstr(full_path, "Windows\\Shell\\BagMRU")) {
        if (debug) {
            fprintf(stderr, "%s: Skipping %s due to filter.\n", prior_stem, stem);
        }
        free(full_path);
        return;
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
            DWORD ach_key_len = 0;
            char ach_key[MAX_KEY_LENGTH];
            unsigned i;
            for (i = 0; i < n_sub_keys; i++) {
                ach_key_len = MAX_KEY_LENGTH;
                ret_code = RegEnumKeyEx(
                    hk_out, i, ach_key, &ach_key_len, nullptr, nullptr, nullptr, nullptr);
                if (ret_code == ERROR_SUCCESS) {
                    write_reg_commands(hk_out, ach_key, max_depth, depth + 1, full_path, debug);
                } else {
                    if (debug) {
                        fprintf(stderr,
                                "%s: Skipping %s because RegEnumKeyEx() failed.\n",
                                prior_stem,
                                full_path);
                    }
                }
            }
        } else if (debug) {
            fprintf(stderr, "%s: No subkeys at %s.\n", prior_stem, stem);
        }
        if (n_values) {
            do_write_reg_commands(hk_out, n_values, full_path, debug);
        } else if (debug) {
            fprintf(stderr, "%s: No values at %s.\n", prior_stem, stem);
        }
        RegCloseKey(hk_out);
    } else {
        if (debug) {
            fprintf(stderr, "%s: Skipping %s. Does the location exist?\n", prior_stem, stem);
        }
    }
    free(full_path);
}

int save_preferences(
    bool commit, const char *deploy_key, const char *output_dir, int max_depth, HKEY hk) {
    (void)commit;
    (void)deploy_key;
    (void)output_dir;
    // if deploy_key -> resolved_deploy_key = resolve_path(deploy_key)
    // mkdir(output_dir)
    write_reg_commands(hk, nullptr, max_depth, 0, "HKCU", false);
    // if commit and has_git
    return 0;
}

int wmain(int argc, wchar_t *argv[]) {
    (void)argc;
    (void)argv;
    save_preferences(false, nullptr, nullptr, 20, HKEY_CURRENT_USER);
    return 0;
}
