/** \file */
#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

//! Escapes a string for a batch file.
/*!
 \param input Input string.
 \param n_chars Length of the input string in word size.
 \return Pointer to escaped string. Must be freed on caller side.
 \ingroup A
 */
wchar_t *escape_for_batch(const wchar_t *input, size_t n_chars);

#endif // SHELL_H
