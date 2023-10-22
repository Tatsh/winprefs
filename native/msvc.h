#ifndef MSVC_H
#define MSVC_H

#ifdef _MSC_VER
#define nullptr NULL
#endif

#ifndef _MSC_VER
#include <stdbool.h>
#else
#define bool BOOL
#define true TRUE
#define false FALSE
#endif

#endif // MSVC_H
