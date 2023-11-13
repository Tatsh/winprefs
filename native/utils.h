#ifndef UTILS_H
#define UTILS_H

static inline void free_if_not_null(void *p) {
    if (p) {
        free(p);
    }
}

#endif // UTILS_H