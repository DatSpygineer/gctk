#pragma once

#include <gctk_api.hpp>
#include <gctk_version.hpp>
#include <gctk_math.hpp>
#include <gctk_cvar.hpp>
#include <gctk_debug.hpp>
#include <gctk_filesys.hpp>
#include <gctk_str.hpp>
#include <gctk_time.hpp>
#include <gctk_asset.hpp>

#ifdef GCTK_CLIENT
	#include <gctk_input.hpp>
	#include <gctk_client.hpp>
#else
	#include <gctk_server.hpp>
#endif