#include <setjmp.h>
#include <stddef.h>

#include <cmocka.h>

#include "io.h"
#include "macros.h"

bool ends_with_ext_gcda(const char *s) {
    return strlen(s) > 5 && !strcmp(s + strlen(s) - 5, ".gcda");
}

bool is_gcda(FILE *restrict stream) {
    (void)stream;
    // char proc_lnk[32];
    // ssize_t r;
    // char filename[PATH_MAX];
    // int fno = fileno(stream);
    // if (fno >= 0) {
    //     sprintf(proc_lnk, "/proc/self/fd/%d", fno);
    //     r = readlink(proc_lnk, filename, PATH_MAX);
    //     if (r < 0) {
    //         return false;
    //     }
    //     filename[r] = '\0';
    //     return ends_with_ext_gcda(filename);
    // }
    return false;
}

FILE *__wrap_fopen(const char *restrict path, const char *restrict mode) {
    if (ends_with_ext_gcda(path)) {
        return __real_fopen(path, mode);
    }
    check_expected(path);
    check_expected(mode);
    return mock_type(FILE *);
}

int __wrap_fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream) {
    if (is_gcda(stream)) {
        return __real_fread(ptr, size, nitems, stream);
    }
    memcpy(ptr, ((fake_file_t *)stream)->data, size);
    check_expected(size);
    check_expected(nitems);
    check_expected(stream);
    return (int)mock();
}

int __wrap_fseek(FILE *stream, long offset, int whence) {
    if (is_gcda(stream)) {
        return __real_fseek(stream, offset, whence);
    }
    check_expected(stream);
    check_expected(offset);
    check_expected(whence);
    return (int)mock();
}

long __wrap_ftell(FILE *stream) {
    if (is_gcda(stream)) {
        return __real_ftell(stream);
    }
    check_expected(stream);
    return (int)mock();
}

void __wrap_rewind(FILE *stream) {
    if (is_gcda(stream)) {
        __real_rewind(stream);
        return;
    }
    check_expected(stream);
}

int __wrap_fclose(FILE *stream) {
    if (is_gcda(stream)) {
        return __real_fclose(stream);
    }
    check_expected(stream);
    return (int)mock();
}
