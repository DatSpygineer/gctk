#ifndef GCTK_COMMON_H
#define GCTK_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#if defined(GCTK_ENABLE_PREPROCESSOR) && (GCTK_ENABLE_PREPROCESSOR != 0)
	#include "gctk/preprocessor.h"
#endif

#ifdef _WIN32
	#ifdef max
		#undef max
	#endif
	#ifdef min
		#undef min
	#endif
#endif

#ifndef __GNUC__
	#if SIZE_MAX == UINT64_MAX
		typedef int64_t ssize_t;
	#else
		typedef int32_t ssize_t;
	#endif
#endif

#ifdef GCTK_STATIC
	#define GCTK_API extern
	#define GCTK_API_CONST GCTK_API const
#else
	#ifdef GCTK_EXPORTS
		#ifdef __GNUC__
			#define GCTK_API __attribute__((visibility("default")))
		#else
			#define GCTK_API __declspec(dllexport)
		#endif
		#define GCTK_API_CONST GCTK_API extern const
	#else
		#ifdef __GNUC__
			#define GCTK_API extern
			#define GCTK_API_CONST extern const
		#else
			#define GCTK_API __declspec(dllimport)
			#define GCTK_API_CONST GCTK_API const
		#endif
	#endif
#endif

#define GCTK_STRCAT_IMPL(__a__, __b__) __a__##__b__
#define GCTK_STRCAT(__a__, __b__) GCTK_STRCAT_IMPL(__a__, __b__)
#define GCTK_UNIQUE_NAME(__prefix__) GCTK_STRCAT(__prefix__, __LINE__)

#define GCTK_DEFER(__free_fn__) \
for (int GCTK_UNIQUE_NAME(__temp) = 0; GCTK_UNIQUE_NAME(__temp) < 1; (GCTK_UNIQUE_NAME(__temp)++, (__free_fn__)))

#define GCTK_MALLOC(T) ((T*)malloc(sizeof(T)))
#define GCTK_CALLOC(T, __count) ((T*)calloc((__count), sizeof(T)))

#endif // GCTK_COMMON_H