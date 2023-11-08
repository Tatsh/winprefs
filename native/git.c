#include "git.h"
#include "constants.h"

static inline bool has_git() {
    return _wspawnlp(P_WAIT, L"git.exe", L"git", L"--version", nullptr) >= 0;
}

static inline bool dir_exists(wchar_t *path) {
    DWORD attrib = GetFileAttributes(path);
    return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool git_commit(const wchar_t *output_dir, const wchar_t *deploy_key) {
    bool ret = true;
    wchar_t *cwd, *date_buf, *git_dir, *git_dir_arg, *message_buf, *time_buf, *work_tree_arg;
    cwd = date_buf = git_dir = git_dir_arg = message_buf = time_buf = work_tree_arg = nullptr;
    if (!has_git()) {
        debug_print(L"Wanted to commit but git.exe is not in PATH or failed to run.\n");
        goto fail;
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
        if (!_wgetcwd(cwd, MAX_PATH)) {
            goto fail;
        }
        if (_wchdir(output_dir) != 0) {
            goto fail;
        }
        if (_wspawnlp(P_WAIT, L"git.exe", L"git", L"init", L"--quiet", nullptr) != 0) {
            goto fail;
        }
        if (_wchdir(cwd) != 0) {
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
    if (_wspawnlp(P_WAIT, L"git.exe", L"git", git_dir_arg, work_tree_arg, L"add", L".", nullptr) !=
        0) {
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
               L"\"%ls%ls %ls\"",
               AUTOMATIC_COMMIT_MESSAGE_PREFIX,
               date_buf,
               time_buf);
    if (_wspawnlp(P_WAIT,
                  L"git.exe",
                  L"git",
                  git_dir_arg,
                  work_tree_arg,
                  L"commit",
                  L"--no-gpg-sign",
                  L"--quiet",
                  L"--no-verify",
                  L"\"--author=winprefs <winprefs@tat.sh>\"",
                  L"-m",
                  message_buf,
                  nullptr) != 0) {
        goto fail;
    }
    if (deploy_key) {
        wchar_t full_deploy_key_path[MAX_PATH];
        if (!_wfullpath(full_deploy_key_path, deploy_key, MAX_PATH)) {
            goto fail;
        }
        debug_print(L"Deploy key: %ls\n", full_deploy_key_path);
        size_t ssh_command_len = 68 + wcslen(full_deploy_key_path) + 3;
        wchar_t *ssh_command = calloc(ssh_command_len, WL);
        if (!ssh_command) { // LCOV_EXCL_START
            goto fail;
        } // LCOV_EXCL_STOP
        wmemset(ssh_command, L'\0', ssh_command_len);
        _snwprintf(ssh_command,
                   ssh_command_len,
                   L"\"ssh -i %ls -F nul -o UserKnownHostsFile=nul -o StrictHostKeyChecking=no\"",
                   full_deploy_key_path);
        if (_wspawnlp(P_WAIT,
                      L"git.exe",
                      L"git",
                      git_dir_arg,
                      work_tree_arg,
                      L"config",
                      L"core.sshCommand",
                      ssh_command,
                      nullptr) != 0) {
            goto fail;
        }
        wchar_t *branch_arg =
            get_git_branch(git_dir_arg, git_dir_arg_len, work_tree_arg, work_tree_arg_len);
        debug_print(L"git.exe \"%ls\" \"%ls\" push -u --porcelain --no-signed origin origin %ls\n",
                    git_dir_arg,
                    work_tree_arg,
                    branch_arg);
        if (_wspawnlp(P_WAIT,
                      L"git.exe",
                      L"git",
                      git_dir_arg,
                      work_tree_arg,
                      L"push",
                      L"-u",
                      L"--porcelain",
                      L"--no-signed",
                      L"origin",
                      L"origin",
                      branch_arg,
                      nullptr) != 0) {
            goto fail;
        }
    }
    goto cleanup;
fail:
    ret = false;
cleanup:
    free_if_not_null(cwd);
    free_if_not_null(date_buf);
    free_if_not_null(git_dir);
    free_if_not_null(git_dir_arg);
    free_if_not_null(message_buf);
    free_if_not_null(time_buf);
    free_if_not_null(work_tree_arg);
    return ret;
}

// Based on https://stackoverflow.com/a/35658917/374110
wchar_t *get_git_branch(const wchar_t *git_dir_arg,
                        size_t git_dir_arg_len,
                        const wchar_t *work_tree_arg,
                        size_t work_tree_arg_len) {
    char *result = malloc(255);
    wchar_t *cmd, *w_result;
    cmd = w_result = nullptr;
    if (!result) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    memset(result, 0, 255);
    HANDLE pipe_read, pipe_write;
    SECURITY_ATTRIBUTES sa_attr = {.lpSecurityDescriptor = nullptr,
                                   .bInheritHandle =
                                       true, // Pipe handles are inherited by child process.
                                   .nLength = sizeof(SECURITY_ATTRIBUTES)};
    // Create a pipe to get results from child's stdout.
    if (!CreatePipe(&pipe_read, &pipe_write, &sa_attr, 0)) {
        goto fail;
    }
    STARTUPINFO si = {.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES,
                      .hStdOutput = pipe_write,
                      .hStdError = pipe_write,
                      .wShowWindow = SW_HIDE};
    PROCESS_INFORMATION pi = {0};
    size_t cmd_len = git_dir_arg_len + work_tree_arg_len + 30;
    cmd = calloc(cmd_len, WL);
    if (!cmd) { // LCOV_EXCL_START
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(cmd, L'\0', cmd_len);
    _snwprintf(
        cmd, cmd_len, TEXT("git.exe %ls %ls branch --show-current"), git_dir_arg, work_tree_arg);
    cmd[cmd_len - 1] = L'\0';
    bool ret = CreateProcess(
        nullptr, cmd, nullptr, nullptr, true, CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);
    if (!ret) {
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        goto fail;
    }
    free(cmd);
    bool proc_ended = false;
    for (; !proc_ended;) {
        // Give some time slice (50 ms), so we won't waste 100% CPU.
        proc_ended = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;
        // Even if process exited - we continue reading, if
        // there is some data available over pipe.
        for (;;) {
            char buf[255];
            memset(buf, L'\0', 255);
            DWORD read = 0;
            DWORD avail = 0;
            if (!PeekNamedPipe(pipe_read, nullptr, 0, nullptr, &avail, nullptr)) {
                break;
            }
            if (!avail) { // No data available, return
                break;
            }
            if (!ReadFile(pipe_read, buf, min(sizeof(buf) - 1, avail), &read, nullptr) || !read) {
                // Error, the child process might have ended
                break;
            }
            buf[min(sizeof(buf) - 1, avail)] = L'\0';
            if (avail) {
                strncat(result, buf, proc_ended ? avail - 1 : avail); // Strip newline
            }
        }
    }
    CloseHandle(pipe_write);
    CloseHandle(pipe_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    size_t res_len = strlen(result);
    int w_len = MultiByteToWideChar(CP_UTF8, 0, result, (int)res_len, nullptr, 0);
    w_result = calloc((size_t)w_len + 1, WL);
    if (!w_result) { // LCOV_EXCL_START
        goto fail;
    } // LCOV_EXCL_STOP
    MultiByteToWideChar(CP_UTF8, 0, result, (int)res_len, w_result, w_len);
    w_result[w_len] = L'\0';
    goto cleanup;
fail:
    w_result = nullptr;
cleanup:
    free_if_not_null(cmd);
    free_if_not_null(result);
    return w_result;
}
