#pragma once

#include <stdint.h>

/* Define platform specific types */
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;

typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;

typedef uintptr_t mem_ptr_t;

/* printf formatting */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hX"
#define U32_F "u"
#define S32_F "d"
#define X32_F "X"
#define SZT_F "uz"

/* Byte order */
#define BYTE_ORDER LITTLE_ENDIAN

// Define macros for efficient byte swapping

/* Checksum algorithm */
#define LWIP_CHKSUM_ALGORITHM 2

/* Alignment */
#define MEM_ALIGNMENT 4

/* Packing */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* Diagnostics */
#define LWIP_PLATFORM_DIAG(x)

/* Assertions */
#define LWIP_PLATFORM_ASSERT(x)

// TEST TODO: REMOVE 