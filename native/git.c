#include "git.h"
#include "constants.h"
#include "git_branch.h"

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
            _wspawnlp(P_WAIT, L"git.exe", L"git", L"init", L"--quiet", nullptr) != 0 ||
            _wchdir(cwd) != 0) {
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
