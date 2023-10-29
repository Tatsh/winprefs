#ifndef MACROS_H
#define MACROS_H

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L)
#include <stddef.h>
#else
#define nullptr (void *)0
#endif

#ifndef _MSC_VER
#include <stdbool.h>
#else
#define bool BOOL
#define true TRUE
#define false FALSE
#endif

#ifndef _WC_ERR_INVALID_CHARS
#define _WC_ERR_INVALID_CHARS 0x0080
#endif

#endif // MACROS_H
