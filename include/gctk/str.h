#ifndef GCTK_STR_H
#define GCTK_STR_H

#include "gctk/common.h"

#include <string.h>

#define GCTK_NOT_FOUND -1

#ifndef hash_t
	#define hash_t uint32_t
#endif

#ifndef GCTK_CRC32_POLY
	#define GCTK_CRC32_POLY 0xEDB88320
#endif

#define GctkCharIsWhitespace(__c) ((__c) == ' ' || (__c) == '\t' || (__c) == '\n' || (__c) == '\r')
#define GctkCharIsLetterLowercase(__c) (((__c) >= 'a' && (__c) <= 'z'))
#define GctkCharIsLetterUppercase(__c) (((__c) >= 'A' && (__c) <= 'Z'))
#define GctkCharIsLetter(__c) (GctkCharIsLetterLowercase(__c) || GctkCharIsLetterUppercase(__c))
#define GctkCharIsNumber(__c) ((__c) >= '0' && (__c) <= '9')
#define GctkCharIsNumberHex(__c) (((__c) >= '0' && (__c) <= '9') || ((__c) >= 'a' && (__c) <= 'f') || ((__c) >= 'A' && (__c) <= 'F'))
#define GctkCharIsNumberOct(__c) ((__c) >= '0' && (__c) <= '7')
#define GctkCharIsNumberBin(__c) ((__c) == '0' || (__c) == '1')
#define GctkCharIsAlphaNumeric(__c) (GctkCharIsNumber(__c) || GctkCharIsLetter(__c))

GCTK_API hash_t (*GctkStrHashFunction)(const char *str);

GCTK_API char* GctkStrCpy(char* trg, const char* source, size_t max_size);
GCTK_API char* GctkStrCpySlice(char* trg, const char* source, size_t max_size, size_t count);
GCTK_API char* GctkStrJoin(char* trg, const char* lhs, const char* rhs, size_t max_size);
GCTK_API char* GctkStrAppend(char* trg, const char* src, size_t max_size);

GCTK_API bool GctkStrStartWith(const char* str, char start_with);
GCTK_API bool GctkStrEndWith(const char* str, char end_with);

GCTK_API bool GctkStrStartWithStr(const char* str, const char* start_with);
GCTK_API bool GctkStrStartWithStrNoCase(const char* str, const char* start_with);
GCTK_API bool GctkStrEndWithStr(const char* str, const char* end_with);
GCTK_API bool GctkStrEndWithStrNoCase(const char* str, const char* end_with);

GCTK_API bool GctkStrEq(const char* lhs, const char* rhs);
GCTK_API bool GctkStrEqNoCase(const char* lhs, const char* rhs);

GCTK_API bool GctkStrEqSlice(const char* lhs, const char* rhs, size_t n);
GCTK_API bool GctkStrEqSliceNoCase(const char* lhs, const char* rhs, size_t n);

GCTK_API bool GctkStrIsValidInteger(const char* str, int base);
GCTK_API bool GctkStrIsValidNumber(const char* str);

GCTK_API bool GctkStrParseToInt(const char* str, int base, int* output);
GCTK_API bool GctkStrParseToFloat(const char* str, float* output);
GCTK_API bool GctkStrParseToInt64(const char* str, int base, int64_t* output);
GCTK_API bool GctkStrParseToFloat64(const char* str, double* output);

GCTK_API bool GctkStrParseSliceToInt(const char* str, size_t length, int base, int* output);
GCTK_API bool GctkStrParseSliceToFloat(const char* str, size_t length, float* output);

GCTK_API ssize_t GctkStrFind(const char* str, char c);
GCTK_API ssize_t GctkStrFindStr(const char* str, const char* to_find);
GCTK_API ssize_t GctkStrFindAny(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindLast(const char* str, char c);
GCTK_API ssize_t GctkStrFindLastStr(const char* str, const char* to_find);
GCTK_API ssize_t GctkStrFindAnyLast(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindAnyNot(const char* str, const char* chars);
GCTK_API ssize_t GctkStrFindAnyLastNot(const char* str, const char* chars);

GCTK_API hash_t GctkStrHash(const char* cstr);

GCTK_API void* GctkMemDup(const void* original, size_t size, size_t count);

#endif