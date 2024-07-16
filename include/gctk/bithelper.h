#ifndef GCTK_BIT_HELPER
#define GCTK_BIT_HELPER

#include "gctk/common.h"

typedef enum Endianness {
	GCTK_LITTLE_ENDIAN,
	GCTK_BIG_ENDIAN
} Endianness;

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define GCTK_ENDIANNESS GCTK_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define GCTK_ENDIANNESS GCTK_BIG_ENDIAN
#else
	#error Could not determine endianness
#endif

GCTK_API uint8_t GctkReverseBits(uint8_t value);

GCTK_API uint16_t GctkReverseU16(uint16_t value);
GCTK_API uint32_t GctkReverseU32(uint32_t value);
GCTK_API uint64_t GctkReverseU64(uint64_t value);
GCTK_API int16_t GctkReverseI16(int16_t value);
GCTK_API int32_t GctkReverseI32(int32_t value);
GCTK_API int64_t GctkReverseI64(int64_t value);

#endif