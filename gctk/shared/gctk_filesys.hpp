#pragma once

#include <filesystem>

namespace gctk {
	using Path = std::filesystem::path;

	namespace Paths {
		void init(const std::string& argv0, const std::string& name);
		Path base_path();
		Path bin_path();
		Path cfg_path();
		Path res_path();
		Path user_path();

		using namespace std::filesystem;
	}
}
