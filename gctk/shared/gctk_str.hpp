#pragma once

#include <string>
#include <vector>

namespace gctk::StringUtil {
	std::string ToLower(const std::string& str);
	std::string ToUpper(const std::string& str);
	std::string TrimBeg(const std::string& str, const std::string& chars = " \t\n\r");
	std::string TrimEnd(const std::string& str, const std::string& chars = " \t\n\r");
	std::string Trim(const std::string& str, const std::string& chars = " \t\n\r");
	std::vector<std::string> Split(const std::string& str, char delimiter);
	std::vector<std::string> SplitLines(const std::string& str);
	std::string Join(const std::vector<std::string>& strings, char delimiter);
	std::string Join(const std::vector<std::string>& strings, const std::string& delimiter);
}
