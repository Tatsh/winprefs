#include "git_branch.h"
#include "constants.h"

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
    size_t cmd_len = git_dir_arg_len + work_tree_arg_len + 34;
    cmd = calloc(cmd_len, WL);
    if (!cmd) { // LCOV_EXCL_START
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        goto fail;
    } // LCOV_EXCL_STOP
    wmemset(cmd, L'\0', cmd_len);
    _snwprintf(cmd,
               cmd_len,
               TEXT("git.exe \"%ls\" \"%ls\" branch --show-current"),
               git_dir_arg,
               work_tree_arg);
    cmd[cmd_len - 1] = L'\0';
    debug_print(L"Executing: '%ls'\n", cmd);
    bool ret = CreateProcess(
        nullptr, cmd, nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    if (!ret) {
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        goto fail;
    }
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
            if (!PeekNamedPipe(pipe_read, nullptr, 0, nullptr, &avail, nullptr) || !avail ||
                !ReadFile(pipe_read, buf, min(sizeof(buf) - 1, avail), &read, nullptr) || !read) {
                break;
            }
            buf[min(sizeof(buf) - 1, avail)] = L'\0';
            if (avail) {
                strncat(result, buf, avail);
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
    StrTrimW(w_result, L"\r\n\t ");
    goto cleanup;
fail:
    w_result = nullptr;
cleanup:
    free(cmd);
    free(result);
    return w_result;
}
