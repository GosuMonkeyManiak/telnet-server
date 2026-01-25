#pragma once

#include <stdio.h>
#include <sys/time.h>

#include <xc.h>

#include "debug_printf.h"

/* ### Non-standard functions ### */

/* lwip_itoa replacement */
#define lwip_itoa(result, bufsize, number) snprintf(result, bufsize, "%d", number)

/* use default: lwip_memcmp_consttime() */

/* use default: lwip_stricmp() */

/* use default: lwip_strnicmp() */

/* use default: lwip_strnistr() */

/* use default: lwip_strnstr() */

/* ### Compiler/platform abstraction ### */

/* Byte order */
//#define BYTE_ORDER LITTLE_ENDIAN

/* Random number */
//#define LWIP_RAND() ((u32_t)rand())

#ifdef DEBUG_MODE

/* Diagnostic output */
#define LWIP_PLATFORM_DIAG(message) \
	do { \
		debug_print message ; \
		debug_print("\r\n"); \
	} while(0)

/* Assertion handling */
#define LWIP_PLATFORM_ASSERT(message) \
	do { \
		debug_print("\r\n\r\n ASSERTION \"%s\" failed at line %d in %s!\r\n\r\n", message, __LINE__, __FILE__); \
		__builtin_software_breakpoint(); \
	} while(0)

#else

#define LWIP_NOASSERT
#define LWIP_PLATFORM_DIAG(message)
#define LWIP_PLATFORM_ASSERT(message)

#endif

/* Standard define inclusion */
#define LWIP_NO_STDDEF_H 0

/* Standard int inclusion */
#define LWIP_NO_STDINT_H 0

/* Standard inttypes inclusion */
#define LWIP_NO_INTTYPES_H 0

/* Standard limits inclusion */
#define LWIP_NO_LIMITS_H 0

/* Standard ctype inclusion */
#define LWIP_NO_CTYPE_H 0

/* Cast LWIP_COSNT_CAST used ptrdiff_t --> alltypes.h */

/* Aligment cast LWIP_ALIGNMENT_CAST uses LWIP_COSNT_CAST */

/* Pointer numberic cast LWIP_PTR_NUMERIC_CAST uses LWIP_COSNT_CAST */

/* Packed cast LWIP_PACKED_CAST uses LWIP_COSNT_CAST */

/* Declare memory aligned LWIP_DECLARE_MEMORY_ALIGNED */
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[size] __attribute__((aligned(4)))

/* Memory aligned size LWIP_MEM_ALIGN_SIZE */

/* Memory aligned buffer LWIP_MEM_ALIGN_BUFFER */

/* Memory aligned LWIP_MEM_ALIGN */

/* Pack struct begin */
#define PACK_STRUCT_BEGIN

/* Pack struct end */
#define PACK_STRUCT_END

/* Pack struct struct */
#define PACK_STRUCT_STRUCT __attribute__((packed))

/* Pack struct field */
#define PACK_STRUCT_FIELD(x) x

/* Pack struct FLD_8 uses PACK_STRUCT_FIELD */

/* Pack struct FLD_S uses PACK_STRUCT_FIELD */

/* Pack struct use includes */
//#define PACK_STRUCT_USE_INCLUDES 0

/* Unused argument */
#define LWIP_UNUSED_ARG(x) (void)x

/* LWIP provide errno */
#define LWIP_PROVIDE_ERRNO 1

