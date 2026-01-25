#pragma once

#include <stdio.h>

void debug_printf(const char *format, ...);

#if DEBUG_MODE

#define debug_print printf

#else

#define debug_print(format, ...)

#endif