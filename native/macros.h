#ifndef MACROS_H
#define MACROS_H

#if !HAVE_SNWPRINTF
#define _snwprintf swprintf
#endif
#if !HAVE_WCSNICMP
#define _wcsnicmp wcsncmp
#endif
#if !HAVE_WCSICMP
#define _wcsicmp wcscmp
#endif

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

#endif // MACROS_H
