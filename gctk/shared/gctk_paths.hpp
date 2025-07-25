#pragma once

#include "gctk_api.hpp"

#include <filesystem>

namespace gctk::Paths {
	GCTK_API void init(int argc, char** argv);
	GCTK_API std::filesystem::path base_path();
	GCTK_API std::filesystem::path bin_path();
	GCTK_API std::filesystem::path cfg_path();
	GCTK_API std::filesystem::path res_path();
	GCTK_API std::filesystem::path user_path();
}
