#include "calloc.h"

void *__wrap_calloc(void *buf, size_t n_size) {
    if (want_calloc_abort) {
        return nullptr;
    }
    return __real_calloc(buf, n_size);
}
