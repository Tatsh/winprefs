#include <stdarg.h>
#include <stdio.h>

#include "debug.h"

void debug_print(const wchar_t *format, ...) {
    if (debug_print_enabled) {
        va_list args;
        va_start(args, format);
        vfwprintf(stderr, format, args);
        va_end(args);
    }
}
