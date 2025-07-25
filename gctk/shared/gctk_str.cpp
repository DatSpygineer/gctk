#include "gctk_str.hpp"

#include <sstream>
#include <algorithm>

namespace gctk::StringUtil {
	std::string ToLower(const std::string& str) {
		std::string result = str;
		for (auto& c : result) {
			c = static_cast<char>(std::tolower(c));
		}
		return result;
	}
	std::string ToUpper(const std::string& str) {
		std::string result = str;
		for (auto& c : result) {
			c = static_cast<char>(std::toupper(c));
		}
		return result;
	}
	std::string TrimBeg(const std::string& str, const std::string& chars) {
		if (const auto idx = str.find_first_not_of(chars); idx != std::string::npos) {
			return str.substr(idx);
		}
		return "";
	}
	std::string TrimEnd(const std::string& str, const std::string& chars) {
		if (const auto idx = str.find_last_not_of(chars); idx != std::string::npos) {
			return str.substr(0, idx + 1);
		}
		return "";
	}
	std::string Trim(const std::string& str, const std::string& chars) {
		return TrimEnd(TrimBeg(str, chars), chars);
	}
	std::vector<std::string> Split(const std::string& str, const char delimiter) {
		std::vector<std::string> result;
		std::istringstream iss(str);
		std::string token;
		while (std::getline(iss, token, delimiter)) {
			result.emplace_back(token);
		}
		return result;
	}
	std::vector<std::string> SplitLines(const std::string& str) {
		std::vector<std::string> result;
		std::istringstream iss(str);
		std::string token;
		while (std::getline(iss, token)) {
			result.emplace_back(token);
		}
		return result;
	}

	std::string Join(const std::vector<std::string>& strings, char delimiter) {
		std::ostringstream oss;
		for (const auto& str : strings) {
			if (oss.tellp() > 0) {
				oss << delimiter;
			}
			oss << str;
		}
		return oss.str();
	}
	std::string Join(const std::vector<std::string>& strings, const std::string& delimiter) {
		std::ostringstream oss;
		for (const auto& str : strings) {
			if (oss.tellp() > 0) {
				oss << delimiter;
			}
			oss << str;
		}
		return oss.str();
	}
}
