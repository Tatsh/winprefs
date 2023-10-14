#include "unicode.h"

#include <stdio.h>
#include <wchar.h>

#include <shlwapi.h>
#include <windows.h>

#include "arg.h"
#include "constants.h"
#include "reg_command.h"

void write_reg_commands(
    HKEY hk, const wchar_t *stem, int max_depth, int depth, const wchar_t *prior_stem, bool debug) {
    if (depth >= max_depth) {
        if (debug) {
            fwprintf(stderr,
                     L"%ls: Skipping %ls due to depth limit of %d.\n",
                     prior_stem,
                     stem,
                     max_depth);
        }
        return;
    }
    HKEY hk_out;
    size_t full_path_len = WL * MAX_KEY_LENGTH;
    wchar_t *full_path = calloc(MAX_KEY_LENGTH, WL);
    if (full_path == nullptr) {
        fwprintf(stderr, L"%ls: Stopping due to memory error.\n", prior_stem);
        abort();
    }
    wmemset(full_path, L'\0', MAX_KEY_LENGTH);
    size_t prior_stem_len = wcslen(prior_stem) * WL;
    size_t stem_len = stem ? wcslen(stem) : 0;
    if ((prior_stem_len + (stem_len * WL) + 2) > (full_path_len - 2)) {
        if (debug) {
            fwprintf(
                stderr, L"%ls: Skipping %ls because of length limitation.\n", prior_stem, stem);
        }
        free(full_path);
        return;
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
        wcsstr(full_path, L"Windows\\Shell\\BagMRU")) {
        if (debug) {
            fwprintf(stderr, L"%ls: Skipping %ls due to filter.\n", prior_stem, stem);
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
            wchar_t ach_key[MAX_KEY_LENGTH];
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
                                "%ls: Skipping %ls because RegEnumKeyEx() failed.\n",
                                prior_stem,
                                full_path);
                    }
                }
            }
        } else if (debug) {
            fwprintf(stderr, L"%ls: No subkeys in %ls.\n", prior_stem, stem);
        }
        if (n_values) {
            do_write_reg_commands(hk_out, n_values, full_path, debug);
        } else if (debug) {
            fwprintf(stderr, L"%ls: No values in %ls.\n", prior_stem, stem);
        }
        RegCloseKey(hk_out);
    } else {
        if (debug) {
            fwprintf(stderr, L"%ls: Skipping %ls. Does the location exist?\n", prior_stem, stem);
        }
    }
    free(full_path);
}

int save_preferences(bool commit,
                     const wchar_t *deploy_key,
                     const wchar_t *output_dir,
                     int max_depth,
                     HKEY hk,
                     bool debug) {
    (void)commit;
    if (output_dir) {
        wchar_t full_output_dir[_MAX_PATH];
        if (_wfullpath(full_output_dir, output_dir, _MAX_PATH) == nullptr) {
            abort();
        }
        if (debug) {
            fwprintf(stderr, L"Output directory: %ls\n", full_output_dir);
        }
        // mkdir with parents
    }
    if (deploy_key) {
        wchar_t full_deploy_key_path[_MAX_PATH];
        if (_wfullpath(full_deploy_key_path, deploy_key, _MAX_PATH) == nullptr) {
            abort();
        }
        if (debug) {
            fwprintf(stderr, L"Deploy key: %ls\n", full_deploy_key_path);
        }
    }
    write_reg_commands(hk, nullptr, max_depth, 0, L"HKCU", debug);
    // if commit and has_git
    // if deploy key, then push
    return 0;
}

int wmain(int argc, wchar_t *argv[]) {
    // (void)argc;
    wchar_t *argv0 = argv[0];
    bool commit = false;
    bool debug = false;
    wchar_t *deploy_key = nullptr;
    wchar_t *output_dir = nullptr;
    int max_depth = 20;
    HKEY starting_key = HKEY_CURRENT_USER;
    ARG_BEGIN {
        if (ARG_LONG("deploy-key"))
        case 'K': {
            deploy_key = ARG_VAL();
        }
        else if (ARG_LONG("commit")) case 'c': {
            commit = true;
            ARG_FLAG();
        }
        else if (ARG_LONG("debug")) case 'd': {
            debug = true;
            ARG_FLAG();
        }
        else if (ARG_LONG("output-dir")) case 'o': {
            output_dir = ARG_VAL();
        }
        else if (ARG_LONG("max-depth")) case 'm': {
            wchar_t *val = ARG_VAL();
            size_t w_len = wcslen(val);
            char *as_char = malloc(w_len + 1);
            memset(as_char, 0, w_len + 1);
            wcstombs(as_char, val, w_len);
            max_depth = atoi(as_char);
            free(as_char);
        }
        else if (ARG_LONG("help")) case 'h':
        case '?': {
            PathStripPath(argv0);
            wprintf(L"Usage: %ls [OPTION...] [REG_PATH]\n", argv0);
            puts("Options:");
            puts("  -c, --commit        Commit changes.");
            puts("  -d, --debug         Enable debug logging.");
            puts("  -m, --max-depth=INT Set maximum depth.");
            puts("  -h, --help          Display this help and exit.");
            return EXIT_SUCCESS;
        }
    }
    ARG_END;
    save_preferences(commit, deploy_key, output_dir, max_depth, starting_key, debug);
    return EXIT_SUCCESS;
}
