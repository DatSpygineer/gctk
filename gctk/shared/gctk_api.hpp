#pragma once

#ifdef _WIN32
	#define GCTK_GAME_API __declspec(dllexport) extern "C"
#else
	#define GCTK_GAME_API __attribute__((visibility("default"))) extern "C"
#endif