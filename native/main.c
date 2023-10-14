#include <stdio.h>
#include <wchar.h>

#include <pathcch.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>

#include "arg.h"
#include "constants.h"
#include "reg_command.h"

// Based on https://stackoverflow.com/a/35658917/374110
wchar_t *get_git_branch(const wchar_t *git_dir_arg,
                        size_t git_dir_arg_len,
                        const wchar_t *work_tree_arg,
                        size_t work_tree_arg_len) {
    wchar_t *result = calloc(255, WL);
    HANDLE pipe_read, pipe_write;
    SECURITY_ATTRIBUTES sa_attr = {.lpSecurityDescriptor = NULL,
                                   .bInheritHandle =
                                       TRUE, // Pipe handles are inherited by child process.
                                   .nLength = sizeof(SECURITY_ATTRIBUTES)};
    // Create a pipe to get results from child's stdout.
    if (!CreatePipe(&pipe_read, &pipe_write, &sa_attr, 0)) {
        abort();
    }
    STARTUPINFOW si = {.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES,
                       .hStdOutput = pipe_write,
                       .hStdError = pipe_write,
                       .wShowWindow = SW_HIDE};
    PROCESS_INFORMATION pi = {0};
    size_t cmd_len = git_dir_arg_len + work_tree_arg_len + 28;
    wchar_t *cmd = calloc(cmd_len, WL);
    wmemset(cmd, L'\0', cmd_len);
    _snwprintf(cmd, cmd_len, L"git %ls %ls branch --show-current", git_dir_arg, work_tree_arg);
    BOOL ret = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (!ret) {
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        return result;
    }
    bool proc_ended = false;
    for (; !proc_ended;) {
        // Give some time slice (50 ms), so we won't waste 100% CPU.
        proc_ended = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;
        // Even if process exited - we continue reading, if
        // there is some data available over pipe.
        for (;;) {
            wchar_t buf[255];
            DWORD read = 0;
            DWORD avail = 0;
            if (!PeekNamedPipe(pipe_read, NULL, 0, NULL, &avail, NULL)) {
                break;
            }
            if (!avail) { // No data available, return
                break;
            }
            if (!ReadFile(pipe_read, buf, min(sizeof(buf) - 1, avail), &read, NULL) || !read) {
                // Error, the child process might have ended
                break;
            }
            buf[read] = 0;
            wcsncat(result, buf, 255);
        }
    }
    CloseHandle(pipe_write);
    CloseHandle(pipe_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

void write_reg_commands(HKEY hk,
                        const wchar_t *stem,
                        int max_depth,
                        int depth,
                        FILE *out_fp,
                        const wchar_t *prior_stem,
                        bool debug) {
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
                    write_reg_commands(
                        hk_out, ach_key, max_depth, depth + 1, out_fp, full_path, debug);
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
            do_write_reg_commands(out_fp, hk_out, n_values, full_path, debug);
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
    wchar_t full_output_dir[MAX_PATH];
    if (_wfullpath(full_output_dir, output_dir, MAX_PATH) == nullptr) {
        abort();
    }
    if (debug) {
        fwprintf(stderr, L"Output directory: %ls\n", full_output_dir);
    }
    SHCreateDirectoryEx(NULL, full_output_dir, NULL);
    PathCchAppend(full_output_dir, MAX_PATH, L"exec-reg.bat");
    wchar_t full_deploy_key_path[MAX_PATH];
    if (deploy_key) {
        if (_wfullpath(full_deploy_key_path, deploy_key, MAX_PATH) == nullptr) {
            abort();
        }
        if (debug) {
            fwprintf(stderr, L"Deploy key: %ls\n", full_deploy_key_path);
        }
    }
    FILE *out_fp = _wfopen(full_output_dir, L"w+");
    write_reg_commands(hk, nullptr, max_depth, 0, out_fp, L"HKCU", debug);
    fclose(out_fp);
    intptr_t has_git = _wspawnl(P_WAIT, L"git.exe", L"-v") == 0;

    if (commit) {
        if (!has_git) {
            fwprintf(stderr, L"Wanted to commit but git.exe is not in Path or failed to run.\n");
            return 0;
        }
        if (debug) {
            fwprintf(stderr, L"Committing changes.");
        }
        size_t work_tree_arg_len = wcslen(output_dir) + wcslen(L"--work-tree=") + 1;
        wchar_t *work_tree_arg = calloc(work_tree_arg_len, WL);
        wmemset(work_tree_arg, L'\0', work_tree_arg_len);
        _snwprintf(work_tree_arg, work_tree_arg_len, L"--work-tree=%ls", output_dir);
        size_t git_dir_arg_len = wcslen(output_dir) + wcslen(L"--git-dir=") + wcslen(L"\\.git") + 1;
        wchar_t *git_dir_arg = calloc(git_dir_arg_len, WL);
        wmemset(git_dir_arg, L'\0', git_dir_arg_len);
        _snwprintf(git_dir_arg, git_dir_arg_len, L"--git-dir=%ls\\.git", output_dir);
        _wspawnl(P_WAIT, L"git.exe", git_dir_arg, work_tree_arg, L"add", L".");
        _wspawnl(P_WAIT,
                 L"git.exe",
                 git_dir_arg,
                 work_tree_arg,
                 L"commit",
                 L"--no-gpg-sign",
                 L"--quiet",
                 L"--no-verify",
                 L"--author=winprefs <winprefs@tat.sh>",
                 L"-m",
                 L"Automatic commit @ ");
        if (deploy_key) {
            size_t ssh_command_len = 68 + wcslen(full_deploy_key_path) + 1;
            wchar_t *ssh_command = calloc(ssh_command_len, WL);
            wmemset(ssh_command, L'\0', ssh_command_len);
            _snwprintf(ssh_command,
                       ssh_command_len,
                       L"ssh -i %ls -F nul -o UserKnownHostsFile=nul -o StrictHostKeyChecking=no",
                       full_deploy_key_path);
            _wspawnl(P_WAIT,
                     L"git.exe",
                     git_dir_arg,
                     work_tree_arg,
                     L"config",
                     L"core.sshCommand",
                     ssh_command);
            wchar_t *branch_arg =
                get_git_branch(git_dir_arg, git_dir_arg_len, work_tree_arg, work_tree_arg_len);
            _wspawnl(P_WAIT,
                     L"git.exe",
                     git_dir_arg,
                     work_tree_arg,
                     L"push",
                     L"-u",
                     L"--porcelain",
                     L"--no-signed",
                     L"origin",
                     L"origin",
                     branch_arg);
            free(ssh_command);
            free(branch_arg);
        }
        free(git_dir_arg);
        free(work_tree_arg);
    }
    return 0;
}

int wmain(int argc, wchar_t *argv[]) {
    // (void)argc;
    wchar_t *argv0 = argv[0];
    bool commit = false;
    bool debug = false;

    wchar_t *output_dir = nullptr;
    wchar_t *deploy_key = nullptr;

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
    if (!output_dir) {
        output_dir = calloc(MAX_PATH, sizeof(wchar_t));
        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, output_dir))) {
            PathCchAppend(output_dir, MAX_PATH, L"prefs-export");
        }
    }
    save_preferences(commit, deploy_key, output_dir, max_depth, starting_key, debug);
    free(output_dir);
    return EXIT_SUCCESS;
}
