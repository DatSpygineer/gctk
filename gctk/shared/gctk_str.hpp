#pragma once

#include <string>
#include <vector>

#include "gctk_api.hpp"

namespace gctk::StringUtil {
	GCTK_API std::string ToLower(const std::string& str);
	GCTK_API std::string ToUpper(const std::string& str);
	GCTK_API std::string TrimBeg(const std::string& str, const std::string& chars = " \t\n\r");
	GCTK_API std::string TrimEnd(const std::string& str, const std::string& chars = " \t\n\r");
	GCTK_API std::string Trim(const std::string& str, const std::string& chars = " \t\n\r");
	GCTK_API std::vector<std::string> Split(const std::string& str, char delimiter);
	GCTK_API std::vector<std::string> SplitLines(const std::string& str);
	GCTK_API std::string Join(const std::vector<std::string>& strings, char delimiter);
	GCTK_API std::string Join(const std::vector<std::string>& strings, const std::string& delimiter);
}
