#pragma once

#ifdef GCTK_CLIENT
	#include "gctk_client_api.hpp"
	#define GCTK_API GCTK_CLIENT_API
#else
	#include "gctk_server_api.hpp"
	#define GCTK_API GCTK_SERVER_API
#endif

#ifdef _WIN32
	#define GCTK_GAME_API __declspec(dllexport)
#else
	#define GCTK_GAME_API __attribute__((visibility("default")))
#endif