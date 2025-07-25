#pragma once

#include <string>

#ifndef GCTK_CLIENT_EXPORTS
	#ifdef _WIN32
		#define GCTK_CLIENT_API __declspec(dllimport)
	#else
		#define GCTK_CLIENT_API
	#endif
#else
	#ifdef _WIN32
		#define GCTK_CLIENT_API __declspec(dllexport)
	#else
		#define GCTK_CLIENT_API __attribute__((visibility("default")))
	#endif
#endif