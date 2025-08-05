#pragma once

#include <filesystem>
#include <thread>

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

template<>
struct std::formatter<std::filesystem::path> : public std::formatter<std::string> {
	template<class FmtContext>
	typename FmtContext::iterator format(const std::filesystem::path& path, FmtContext& ctx) const {
		return std::formatter<std::string>::format(path.string(), ctx);
	}
};