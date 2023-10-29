#ifndef CALLOC_H
#define CALLOC_H

#include <stddef.h>

#include "macros.h"

extern bool want_calloc_abort;
void *__real_calloc(void *buf, size_t n_size);

#endif // CALLOC_H
