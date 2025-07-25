#pragma once

#include <string>

#ifndef GCTK_SERVER_EXPORTS
	#ifdef _WIN32
		#define GCTK_SERVER_API __declspec(dllimport)
	#else
		#define GCTK_SERVER_API
	#endif
#else
	#ifdef _WIN32
		#define GCTK_SERVER_API __declspec(dllexport)
	#else
		#define GCTK_SERVER_API __attribute__((visibility("default")))
	#endif
#endif