#pragma once
#include <cstdint>
#include <format>

namespace gctk {
	struct Version {
		uint32_t major, minor, patch;

		constexpr Version(const uint32_t major, const uint32_t minor, const uint32_t patch = 0) :
			major(major), minor(minor), patch(patch) { }

		constexpr int compare(const Version& other) const {
			if (major == other.major) {
				if (minor == other.minor) {
					if (patch == other.patch) {
						return 0;
					}
					return patch > other.patch ? 1 : -1;
				}
				return minor > other.minor ? 1 : -1;
			}
			return major > other.major ? 1 : -1;
		}

		constexpr bool operator< (const Version& other) const { return compare(other) < 0; }
		constexpr bool operator<= (const Version& other) const { return compare(other) <= 0; }
		constexpr bool operator> (const Version& other) const { return compare(other) > 0; }
		constexpr bool operator>= (const Version& other) const { return compare(other) >= 0; }
		constexpr bool operator== (const Version& other) const { return compare(other) == 0; }
		constexpr bool operator!= (const Version& other) const { return compare(other) != 0; }

		friend std::ostream& operator<<(std::ostream& os, const Version& version);
	};

	inline constexpr auto EngineVersion = Version {
		GCTK_VERSION_MAJOR, GCTK_VERSION_MINOR, GCTK_VERSION_PATCH
	};
}

template<>
struct std::formatter<gctk::Version> {
private:
	bool m_bPrintFull = false;
public:
	template<class ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx) {
		for (auto it = ctx.begin(); it != ctx.end(); ++it) {
			if (*it == 'f' || *it == 'F') {
				m_bPrintFull = true;
				return it;
			}
		}
		return ctx.end();
	}
	template<class FmtContext>
	typename FmtContext::iterator format(const gctk::Version& version, FmtContext& ctx) const {
		std::string result = std::format("{}.{}", version.major, version.minor);
		if (m_bPrintFull || version.patch != 0) {
			result += std::format(".{}", version.patch);
		}

		return std::format_to(ctx.out(), "{}", result);
	}
};
