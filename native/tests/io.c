#include <setjmp.h>
#include <stddef.h>

#include <cmocka.h>
#include <io.h>
#include <windows.h>

#include "io.h"
#include "macros.h"

static bool ends_with_ext_gcda(const char *s) {
    size_t len = strlen(s);
    return len > 5 && !strncmp(s + len - 5, ".gcda", 4);
}

bool is_gcda(FILE *restrict stream) {
    (void)stream;
    HANDLE h_file = (HANDLE)_get_osfhandle(_fileno(stream));
    if ((long long)h_file > 2) {
        FILE_NAME_INFO file_name_info;
        if (GetFileInformationByHandleEx(
                h_file, FileNameInfo, &file_name_info, sizeof(FILE_NAME_INFO))) {
            size_t req_size = wcstombs(nullptr, file_name_info.FileName, 0);
            char *narrow = malloc(req_size);
            memset(narrow, 0, req_size);
            wcstombs(narrow, file_name_info.FileName, req_size);
            bool ret = ends_with_ext_gcda(narrow);
            free(narrow);
            return ret;
        }
    }
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
