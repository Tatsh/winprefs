#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "shell.h"

static const unsigned MAX_DATA_SIZE = 8192;

char *escape_for_batch(const char *input, size_t input_len) {
    if (input == nullptr || input_len == 0) {
        return nullptr;
    }
    unsigned i, j;
    size_t new_len = 0;
    for (i = 0; i < input_len; i++) {
        new_len++;
        if (input[i] == '\\' || input[i] == '%' || (input[i] == '\0' && i < (input_len - 1))) {
            new_len++;
        }
    }
    char *out = malloc(MAX_DATA_SIZE);
    if (!out) {
        abort();
    }
    memset(out, 0, MAX_DATA_SIZE);
    for (i = 0, j = 0; i < input_len && j < new_len && j < MAX_DATA_SIZE; i++, j++) {
        if (input[i] == '\0' && i < (input_len - 1) && j < (new_len - 1)) {
            out[j] = '\\';
            out[++j] = '0';
        } else {
            out[j] = input[i];
            if (input[i] == '"' || input[i] == '%') {
                out[++j] = input[i];
            }
        }
    }
    return out;
}
