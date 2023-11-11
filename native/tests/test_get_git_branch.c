#include "git_branch.h"

void test_get_git_branch_CreatePipe_fails(void **state) {
    will_return(__wrap_CreatePipe, false);
    assert_null(get_git_branch(nullptr, 0, nullptr, 0));
}

void test_get_git_branch_CreateProcess_fails(void **state) {
    will_return(__wrap_CreatePipe, true);
    will_return(__wrap_CreateProcess, false);
    will_return_always(__wrap_CloseHandle, true);
    assert_null(get_git_branch(nullptr, 0, nullptr, 0));
}

void test_get_git_branch(void **state) {
    char *master_nl = malloc(8);
    master_nl[7] = '\0';
    memcpy(master_nl, "master\n", 7);
    wchar_t *w_master = calloc(7, sizeof(wchar_t));
    w_master[6] = L'\0';
    wmemcpy(w_master, L"master", 6);

    will_return(__wrap_CreatePipe, true);
    will_return(__wrap_CreateProcess, true);
    will_return_always(__wrap_CloseHandle, true);
    will_return(__wrap_WaitForSingleObject, WAIT_OBJECT_0);
    will_return(__wrap_PeekNamedPipe, 8);
    will_return(__wrap_PeekNamedPipe, true);
    will_return(__wrap_ReadFile, master_nl);
    will_return(__wrap_ReadFile, 7);
    will_return(__wrap_ReadFile, true);
    will_return(__wrap_PeekNamedPipe, 0);
    will_return(__wrap_PeekNamedPipe, true);
    expect_value(__wrap_MultiByteToWideChar, cchWideChar, 0);
    will_return(__wrap_MultiByteToWideChar, 0);
    will_return(__wrap_MultiByteToWideChar, 6);
    expect_value(__wrap_MultiByteToWideChar, cchWideChar, 6);
    will_return(__wrap_MultiByteToWideChar, w_master);
    will_return(__wrap_MultiByteToWideChar, 6);

    wchar_t *ret = get_git_branch(nullptr, 0, nullptr, 0);
    assert_memory_equal(ret, L"master", 12);

    free(master_nl);
    free(ret);
    free(w_master);
}

const struct CMUnitTest powershell_tests[] = {
    cmocka_unit_test(test_get_git_branch),
    cmocka_unit_test(test_get_git_branch_CreatePipe_fails),
    cmocka_unit_test(test_get_git_branch_CreateProcess_fails),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(powershell_tests, nullptr, nullptr);
}
