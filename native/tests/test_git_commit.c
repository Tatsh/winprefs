#include "git.h"

void test_git_commit_no_git_no_fail(void **state) {
    will_return(__wrap__wspawnlp, -1);
    assert_true(git_commit(nullptr, nullptr));
}

void test_git_commit_no_dot_git_wgetcwd_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, INVALID_FILE_ATTRIBUTES);
    will_return(__wrap__wgetcwd, 0);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_no_dot_git_wchdir_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, INVALID_FILE_ATTRIBUTES);
    will_return(__wrap__wgetcwd, L"");
    will_return(__wrap__wchdir, 1);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_no_dot_git_wchdir_fail_2(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, INVALID_FILE_ATTRIBUTES);
    will_return(__wrap__wgetcwd, L"");
    will_return(__wrap__wchdir, 0);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap__wchdir, 1);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_no_dot_git_wspawnlp_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, INVALID_FILE_ATTRIBUTES);
    will_return(__wrap__wgetcwd, L"");
    will_return(__wrap__wchdir, 0);
    will_return(__wrap__wspawnlp, 1);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_add_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 1);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_GetTimeFormat_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 0);
    assert_false(git_commit(L"output-dir", nullptr));

    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 0);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_GetDateFormat_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 0);
    assert_false(git_commit(L"output-dir", nullptr));

    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 0);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap__wspawnlp, 1);
    assert_false(git_commit(L"output-dir", nullptr));
}

void test_git_commit_deploy_key_wfullpath_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap__wfullpath, 0);
    will_return(__wrap__wfullpath, 0);
    assert_false(git_commit(L"output-dir", L"key"));
}

void test_git_commit_deploy_key_config_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wspawnlp, 1);
    assert_false(git_commit(L"output-dir", L"key"));
}

void test_git_commit_deploy_key_push_fail(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_get_git_branch, L"master");
    will_return(__wrap__wspawnlp, 1);
    assert_false(git_commit(L"output-dir", L"key"));
}

void test_git_commit_deploy_key(void **state) {
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetTimeFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap_GetDateFormat, 10);
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wfullpath, L"");
    will_return(__wrap__wspawnlp, 0);
    will_return(__wrap_get_git_branch, L"master");
    will_return(__wrap__wspawnlp, 0);
    assert_true(git_commit(L"output-dir", L"key"));
}

const struct CMUnitTest git_commit_tests[] = {
    cmocka_unit_test(test_git_commit_GetDateFormat_fail),
    cmocka_unit_test(test_git_commit_GetTimeFormat_fail),
    cmocka_unit_test(test_git_commit_add_fail),
    cmocka_unit_test(test_git_commit_deploy_key),
    cmocka_unit_test(test_git_commit_deploy_key_config_fail),
    cmocka_unit_test(test_git_commit_deploy_key_push_fail),
    cmocka_unit_test(test_git_commit_deploy_key_wfullpath_fail),
    cmocka_unit_test(test_git_commit_fail),
    cmocka_unit_test(test_git_commit_no_dot_git_wchdir_fail),
    cmocka_unit_test(test_git_commit_no_dot_git_wchdir_fail_2),
    cmocka_unit_test(test_git_commit_no_dot_git_wgetcwd_fail),
    cmocka_unit_test(test_git_commit_no_dot_git_wspawnlp_fail),
    cmocka_unit_test(test_git_commit_no_git_no_fail),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(git_commit_tests, nullptr, nullptr);
}
