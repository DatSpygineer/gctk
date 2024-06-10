#ifndef GCTK_STR_H
#define GCTK_STR_H

#include "gctk/common.h"

#include <string.h>

#define GCTK_NOT_FOUND -1

#ifndef GCTK_STR_HASH_METHOD
	#define GCTK_STR_HASH_METHOD GctkHashCRC32
#endif

#ifndef hash_t
	#define hash_t uint32_t
#endif

#ifndef GCTK_CRC32_POLY
	#define GCTK_CRC32_POLY 0xEDB88320
#endif

GCTK_API char* GctkStrCpy(char* trg, const char* source, size_t max_size);
GCTK_API char* GctkStrJoin(char* trg, const char* lhs, const char* rhs, size_t max_size);
GCTK_API char* GctkStrAppend(char* trg, const char* src, size_t max_size);

GCTK_API bool GctkStrStartWith(const char* str, char start_with);
GCTK_API bool GctkStrEndWith(const char* str, char end_with);

GCTK_API ssize_t GctkStrFind(const char* str, char c);
GCTK_API ssize_t GctkStrFindStr(const char* str, const char* to_find);
GCTK_API ssize_t GctkStrFindAny(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindLast(const char* str, char c);
GCTK_API ssize_t GctkStrFindLastStr(const char* str, const char* to_find);
GCTK_API ssize_t GctkStrFindAnyLast(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindAnyNot(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindAnyLastNot(const char* str, const char* chars);

GCTK_API uint32_t GctkHashCRC32(const uint8_t* data, size_t data_size);
GCTK_API hash_t GctkStrHash(const char* cstr);

#endif