#pragma once

#include <filesystem>

namespace gctk {
	using Path = std::filesystem::path;

	namespace Paths {
		void Initialize(const std::string& argv0, const std::string& name);
		Path GameBasePath();
		Path GameBinaryPath();
		Path CfgPath();
		Path ResourcePath();
		Path UserDataPath();

		using namespace std::filesystem;
	}
}
