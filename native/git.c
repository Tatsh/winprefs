#include "git.h"
#include "constants.h"
#include "git_branch.h"

// This function does NOT escape double quotes. Inner double quotes must be escaped by the caller.
static inline bool run_process_no_window(int n_args, wchar_t *arg0, ...) {
    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = {.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES,
                      .wShowWindow = SW_HIDE};
    wchar_t cmd[32767];
    wmemset(cmd, L'\0', 32767);
    va_list args;
    va_start(args, arg0);
    wchar_t *cur;
    size_t i = 0;
    size_t req_size = (size_t)_snwprintf(nullptr, 0, L"\"%ls\" ", arg0);
    _snwprintf(cmd, req_size + 1, L"\"%ls\" ", arg0);
    i += req_size;
    int index = 0;
    for (index = 0; index < (n_args - 1); index++) {
        cur = va_arg(args, wchar_t *);
        if (!cur) { // LCOV_EXCL_START
            break;
        } // LCOV_EXCL_STOP
        req_size = (size_t)_snwprintf(nullptr, 0, L"\"%ls\" ", cur);
        _snwprintf(cmd + i, req_size + 1, L"\"%ls\" ", cur);
        i += req_size;
    }
    va_end(args);
    if (i >= 32767) { // LCOV_EXCL_START
        return false;
    } // LCOV_EXCL_STOP
    cmd[i - 1] = L'\0';
    debug_print(L"Executing: '%ls'\n", cmd);
    bool ret = CreateProcess(
        nullptr, cmd, nullptr, nullptr, false, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    if (ret) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exit_code;
        bool result = GetExitCodeProcess(pi.hProcess, &exit_code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        if (!result) {
            debug_print(L"Failed to get exit code.\n");
            return false;
        }
        debug_print(L"Exit code: %d\n", exit_code);
        return exit_code == 0;
    }
    return ret;
}

static inline bool has_git() {
    return run_process_no_window(2, L"git.exe", L"--version");
}

static inline bool dir_exists(wchar_t *path) {
    DWORD attrib = GetFileAttributes(path);
    return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool git_commit(const wchar_t *output_dir, const wchar_t *deploy_key) {
    bool ret = true;
    wchar_t *cwd, *date_buf, *git_dir, *git_dir_arg, *message_buf, *ssh_command, *time_buf,
        *work_tree_arg;
    cwd = date_buf = git_dir = git_dir_arg = message_buf = ssh_command = time_buf = work_tree_arg =
        nullptr;
    if (!has_git()) {
        debug_print(L"Wanted to commit but git.exe is not in PATH or failed to run.\n");
        goto cleanup;
    }
    debug_print(L"Committing changes.\n");
    size_t work_tree_arg_len = wcslen(output_dir) + wcslen(L"--work-tree=") + 1;
    work_tree_arg = calloc(work_tree_arg_len, WL);
    if (!work_tree_arg) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(work_tree_arg, L'\0', work_tree_arg_len);
    _snwprintf(work_tree_arg, work_tree_arg_len, L"--work-tree=%ls", output_dir);
    size_t git_dir_len = wcslen(output_dir) + wcslen(L"\\.git") + 1;
    git_dir = calloc(git_dir_len, WL);
    if (!git_dir) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    _snwprintf(git_dir, git_dir_len, L"%ls\\.git", output_dir);
    git_dir[git_dir_len - 1] = L'\0';
    if (!dir_exists(git_dir)) {
        cwd = calloc(MAX_PATH, WL);
        if (!cwd) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(cwd, L'\0', MAX_PATH);
        if (!_wgetcwd(cwd, MAX_PATH) || _wchdir(output_dir) != 0 ||
            !run_process_no_window(3, L"git.exe", L"init", L"--quiet") || _wchdir(cwd) != 0) {
            goto fail;
        }
    }
    size_t git_dir_arg_len = git_dir_len + wcslen(L"--git-dir=") + 1;
    git_dir_arg = calloc(git_dir_arg_len, WL);
    if (!git_dir_arg) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(git_dir_arg, L'\0', git_dir_arg_len);
    _snwprintf(git_dir_arg, git_dir_arg_len, L"--git-dir=%ls", git_dir);
    git_dir_arg[git_dir_arg_len - 1] = L'\0';
    if (!run_process_no_window(5, L"git.exe", git_dir_arg, work_tree_arg, L"add", L".")) {
        goto fail;
    }
    size_t time_needed_size =
        (size_t)GetTimeFormat(LOCALE_USER_DEFAULT, 0, nullptr, nullptr, nullptr, 0);
    if (!time_needed_size) {
        goto fail;
    }
    time_buf = calloc(time_needed_size, WL);
    if (!time_buf) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    if (!GetTimeFormat(LOCALE_USER_DEFAULT, 0, nullptr, nullptr, time_buf, (int)time_needed_size)) {
        goto fail;
    }
    size_t date_needed_size =
        (size_t)GetDateFormat(LOCALE_USER_DEFAULT, 0, nullptr, nullptr, nullptr, 0);
    if (!date_needed_size) {
        goto fail;
    }
    date_buf = calloc(date_needed_size, WL);
    if (!date_buf) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    if (!GetDateFormat(LOCALE_USER_DEFAULT, 0, nullptr, nullptr, date_buf, (int)date_needed_size)) {
        goto fail;
    }
    size_t needed_size =
        wcslen(AUTOMATIC_COMMIT_MESSAGE_PREFIX) + 3 + time_needed_size + date_needed_size;
    message_buf = calloc(needed_size, WL);
    if (!message_buf) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(message_buf, L'\0', needed_size);
    _snwprintf(message_buf,
               needed_size,
               L"%ls%ls %ls",
               AUTOMATIC_COMMIT_MESSAGE_PREFIX,
               date_buf,
               time_buf);
    if (!run_process_no_window(10,
                               L"git.exe",
                               git_dir_arg,
                               work_tree_arg,
                               L"commit",
                               L"--no-gpg-sign",
                               L"--quiet",
                               L"--no-verify",
                               L"--author=winprefs <winprefs@tat.sh>",
                               L"-m",
                               message_buf)) {
        goto fail;
    }
    if (deploy_key) {
        wchar_t full_deploy_key_path[MAX_PATH];
        if (!_wfullpath(full_deploy_key_path, deploy_key, MAX_PATH)) {
            goto fail;
        }
        debug_print(L"Deploy key: %ls\n", full_deploy_key_path);
        size_t ssh_command_len = 68 + wcslen(full_deploy_key_path) + 3;
        ssh_command = calloc(ssh_command_len, WL);
        if (!ssh_command) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(ssh_command, L'\0', ssh_command_len);
        _snwprintf(ssh_command,
                   ssh_command_len,
                   L"ssh -i %ls -F nul -o UserKnownHostsFile=nul -o StrictHostKeyChecking=no",
                   full_deploy_key_path);
        if (!run_process_no_window(6,
                                   L"git.exe",
                                   git_dir_arg,
                                   work_tree_arg,
                                   L"config",
                                   L"core.sshCommand",
                                   ssh_command)) {
            goto fail;
        }
        wchar_t *branch_arg =
            get_git_branch(git_dir_arg, git_dir_arg_len, work_tree_arg, work_tree_arg_len);
        if (!run_process_no_window(10,
                                   L"git.exe",
                                   git_dir_arg,
                                   work_tree_arg,
                                   L"push",
                                   L"-u",
                                   L"--porcelain",
                                   L"--no-signed",
                                   L"origin",
                                   L"origin",
                                   branch_arg)) {
            goto fail;
        }
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free(cwd);
    free(date_buf);
    free(git_dir);
    free(git_dir_arg);
    free(message_buf);
    free(ssh_command);
    free(time_buf);
    free(work_tree_arg);
    return ret;
}
