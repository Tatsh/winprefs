#ifndef UTILS_H
#define UTILS_H

static inline void free_if_not_null(void *p) {
    if (p) {
        free(p);
    }
}

static inline size_t determine_multi_sz_size(wchar_t *w_data, size_t hint) {
    if (hint == 0) {
        return 0;
    }
    size_t i;
    // Validate the string has correct null terminators
    for (i = 0; i < (hint - 1); i++) {
        wchar_t a = w_data[i];
        wchar_t b = w_data[i + 1];
        if (a == L'\0' && b == L'\0') {
            return i + 1;
        }
    }
    return 0;
}

#endif // UTILS_H
