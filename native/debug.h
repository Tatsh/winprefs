/** \file */
#ifndef DEBUG_H
#define DEBUG_H

extern bool debug_print_enabled;

//! Print only when debug logging is enabled.
/*!
 \param format Format string.
 */
void debug_print(const wchar_t *format, ...);

#endif
