#pragma once

#include <filesystem>

namespace gctk::Paths {
	void init(int argc, char** argv);
	std::filesystem::path base_path();
	std::filesystem::path bin_path();
	std::filesystem::path cfg_path();
	std::filesystem::path res_path();
	std::filesystem::path user_path();
}
