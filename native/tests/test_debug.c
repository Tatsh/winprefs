int set_debug_print_enabled_true(void **state) {
    debug_print_enabled = true;
    return 0;
}

int set_debug_print_enabled_false(void **state) {
    debug_print_enabled = false;
    return 0;
}

void test_debug_print_when_enabled(void **state) {
    will_return(__wrap_vfwprintf, 0);
    expect_function_call(__wrap_vfwprintf);
    debug_print(L"format");
}

void test_debug_print_does_nothing_when_disabled(void **state) {
    // expect_function_calls(__wrap_vfwprintf, 0); // does not work
    debug_print(L"format");
}

const struct CMUnitTest debug_tests[] = {
    cmocka_unit_test_setup_teardown(
        test_debug_print_when_enabled, set_debug_print_enabled_true, set_debug_print_enabled_false),
    cmocka_unit_test(test_debug_print_does_nothing_when_disabled),
};

int main(int argc, char *argv[]) {
    return cmocka_run_group_tests(debug_tests, nullptr, nullptr);
}
