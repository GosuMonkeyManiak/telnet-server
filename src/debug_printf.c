#include <stdio.h>
#include <stdarg.h>

#include "debug_printf.h"

void debug_printf(const char *format, ...) {
		
	va_list args;
	va_start(args, format);
	
	printf(format, args);
	
	va_end(args);
}