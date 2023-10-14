#ifndef SHELL_H
#define SHELL_H

#include "unicode.h"

wchar_t *escape_for_batch(const wchar_t *input_str, size_t input_len);

#endif // SHELL_H
