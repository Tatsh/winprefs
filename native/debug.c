bool debug_print_enabled = false;

void debug_print(const wchar_t *format, ...) {
    if (debug_print_enabled) { // LCOV_EXCL_START
        va_list args;
        va_start(args, format);
        vfwprintf(stderr, format, args);
        va_end(args);
    } // LCOV_EXCL_STOP
}
