#include "utils.h"

void test_determine_multi_sz_size_hint_0_returns_0(void **state) {
    assert_int_equal(determine_multi_sz_size(nullptr, 0), 0);
}

const struct CMUnitTest utils_tests[] = {
    cmocka_unit_test(test_determine_multi_sz_size_hint_0_returns_0),
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return cmocka_run_group_tests(utils_tests, nullptr, nullptr);
}
