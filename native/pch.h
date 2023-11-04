#ifndef WIN_H
#define WIN_H

#include <errno.h>
#include <setjmp.h>
#if HAVE_STDBOOL
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <crtdbg.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
// Must come after windows.h
#include <versionhelpers.h>

#include "debug.h"
#include "macros.h"

#endif // WIN_H
