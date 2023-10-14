#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "shell.h"

wchar_t *escape_for_batch(const wchar_t *input, size_t n_chars) {
    if (input == nullptr || n_chars == 0) {
        return nullptr;
    }
    unsigned i, j;
    size_t new_n_chars = 0;
    //size_t w_size = n_chars * WL;
    for (i = 0; i < n_chars; i++) {
        new_n_chars++;
        // Last condition is to handle REG_MULTI_SZ string sets
        if (input[i] == L'\\' || input[i] == L'%' || (input[i] == L'\0' && i < (n_chars - 1))) {
            new_n_chars++;
        }
    }
    size_t out_size = (new_n_chars + 1) * WL;
    wchar_t *out = malloc(out_size);
    if (!out) {
        abort();
    }
    if (n_chars == new_n_chars) {
        memcpy(out, input, out_size);
        return out;
    }
    memset(out, 0, out_size);
    for (i = 0, j = 0; i < n_chars && j < new_n_chars; i++, j++) {
        // This condition is to handle REG_MULTI_SZ string sets
        if (input[i] == '\0' && i < (n_chars - 1) && j < (new_n_chars - 1)) {
            out[j] = '\\';
            out[++j] = '0';
        } else {
            out[j] = input[i];
            if (input[i] == L'"' || input[i] == L'%') {
                out[++j] = input[i];
            }
        }
    }
    return out;
}
