#include "gctk_filesys.hpp"

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <pwd.h>
#endif

namespace gctk::Paths {
	static Path s_basepath = current_path().parent_path().parent_path().parent_path();
	static Path s_userpath;

	void init(const std::string& argv0, const std::string& name) {
		s_basepath = Path(argv0).parent_path().parent_path().parent_path();
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

	Path base_path() {
		return s_basepath;
	}
	Path bin_path() {
		return s_basepath / "bin" / (GCTK_OS_NAME "_" GCTK_ARCH_NAME);
	}
	Path cfg_path() {
		return s_basepath / "cfg";
	}
	Path res_path() {
		return s_basepath / "res";
	}
	Path user_path() {
		return s_userpath;
	}
}
