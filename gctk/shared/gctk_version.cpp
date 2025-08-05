#include "gctk_version.hpp"

namespace gctk {
	std::ostream& operator<<(std::ostream& os, const Version& version) {
		os << std::format("{}", version);
		return os;
	}
}
