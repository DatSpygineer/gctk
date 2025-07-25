#include "gctk_paths.hpp"

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <pwd.h>
#endif

namespace gctk::Paths {
	static std::filesystem::path s_basepath = std::filesystem::current_path();
	static std::filesystem::path s_userpath;

	void init(const std::string& argv0, const std::string& name) {
		s_basepath = std::filesystem::path(argv0).parent_path();
#ifdef _WIN32
		std::string userpath_base;
		userpath_base.reserve(MAX_PATH);
		SHGetFolderPathA(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, userpath_base.data());
		s_userpath = userpath_base;
#else
		struct passwd* pw = getpwuid(getuid());
		s_userpath = pw->pw_dir;
#endif
		s_userpath /= name;
	}

	std::filesystem::path base_path() {
		return s_basepath;
	}
	std::filesystem::path bin_path() {
		return s_basepath / "bin" / (GCTK_OS_NAME "_" GCTK_ARCH_NAME);
	}
	std::filesystem::path cfg_path() {
		return s_basepath / "cfg";
	}
	std::filesystem::path res_path() {
		return s_basepath / "res";
	}
	std::filesystem::path user_path() {
		return s_userpath;
	}
}
