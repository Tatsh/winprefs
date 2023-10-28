#ifndef MSVC_H
#define MSVC_H

#ifndef _GCC_NULLPTR_T
#define nullptr (void *)0
typedef void *nullptr_t;
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

#endif // MSVC_H
