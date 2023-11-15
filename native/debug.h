/** \file */
#ifndef DEBUG_H
#define DEBUG_H

#include "macros.h"

extern bool debug_print_enabled;

//! Print only when debug logging is enabled.
/*!
 \param format Format string.
 */
void debug_print(const wchar_t *format, ...);

//! Enable or disable debug printing.
/*!
 \param enabled Flag.
 */
DLL_EXPORT void set_debug_print_enabled(bool enabled);

#endif
