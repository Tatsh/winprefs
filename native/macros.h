#ifndef MACROS_H
#define MACROS_H

#if !HAVE_NULLPTR_T
//! `nullptr` definition if not defined by `stddef.h`.
#define nullptr (void *)0
#endif

#if !HAVE_STDBOOL
#define bool BOOL
#define true TRUE
#define false FALSE
#endif

#ifdef __WINE__
#define IsWindowsVistaOrGreater() 0
#endif

#if !HAVE_WC_ERR_INVALID_CHARS
//! Fallback for pre-Vista. Same value as `WC_ERR_INVALID_CHARS`.
#define WC_ERR_INVALID_CHARS
#endif

#define wc_err_invalid_chars IsWindowsVistaOrGreater() ? WC_ERR_INVALID_CHARS : 0

#endif // MACROS_H
