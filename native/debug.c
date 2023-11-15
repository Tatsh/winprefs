// LCOV_EXCL_START
bool debug_print_enabled = false;

void debug_print(const wchar_t *format, ...) {
    if (debug_print_enabled) {
        va_list args;
        va_start(args, format);
        vfwprintf(stderr, format, args);
        va_end(args);
    }
}
DLL_EXPORT void set_debug_print_enabled(bool enabled) {
    debug_print_enabled = enabled;
}
// LCOV_EXCL_STOP
