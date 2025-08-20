#include "gctk_str.hpp"

#include <sstream>
#include <algorithm>
#include <cstring>
#include <strings.h>

#ifdef _WIN32
	#define strcasecmp _stricmp
#endif

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

	bool EqualsNoCase(const std::string& lhs, const std::string& rhs) {
		return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
	}

	bool Equals(const std::string& lhs, const std::string& rhs) {
		return strcmp(lhs.c_str(), rhs.c_str()) == 0;
	}

	bool ParseBool(const std::string& str, bool& out) {
		const auto str_lc = ToLower(str);
		if (str_lc == "true" || str_lc == "on" || str_lc == "yes") {
			out = true;
			return true;
		}
		if (str_lc == "false" || str_lc == "off" || str_lc == "no") {
			out = false;
			return true;
		}

		if (uint8_t value; ParseInt<uint8_t>(str_lc, value)) {
			out = value != 0;
			return true;
		}
		return false;
	}

	bool ParseVector2(const std::string& str, Vector2& out) {
		const auto tokens = Split(str, ' ');
		if (tokens.size() != 2) {
			return false;
		}
		return ParseFloat(tokens.at(0), out.x) && ParseFloat(tokens.at(1), out.y);
	}
	bool ParseVector3(const std::string& str, Vector3& out) {
		const auto tokens = Split(str, ' ');
		if (tokens.size() != 3) {
			return false;
		}
		return ParseFloat(tokens.at(0), out.x) && ParseFloat(tokens.at(1), out.y) &&
			   ParseFloat(tokens.at(2), out.z);
	}
	bool ParseVector4(const std::string& str, Vector4& out) {
		const auto tokens = Split(str, ' ');
		if (tokens.size() != 4) {
			return false;
		}
		return ParseFloat(tokens.at(0), out.x) && ParseFloat(tokens.at(1), out.y) &&
			   ParseFloat(tokens.at(2), out.z) && ParseFloat(tokens.at(3), out.w);
	}
	bool ParseColor(const std::string& str, Color& out) {
		auto str_t = Trim(str);
		if (str_t.starts_with("rgb")) {
			const auto components = str_t.starts_with("rgba") ? 4 : 3;
			const auto values = Trim(str_t.substr(components), "()");
			const auto tokens = Split(values, ',');
			if (tokens.size() != components) {
				return false;
			}

			uint8_t r, g, b;
			float a;
			if (ParseInt<uint8_t>(tokens.at(0), r) && ParseInt<uint8_t>(tokens.at(1), g) &&
				ParseInt<uint8_t>(tokens.at(2), b)) {
				if (components == 3) {
					a = 1.0f;
				} else {
					if (!ParseFloat<float>(tokens.at(3), a)) {
						return false;
					}
				}

				out = Color::FromRgba(r, g, b);
				out.a = a;
				return true;
			}
			return false;
		}
		if (str_t.starts_with("hsl")) {
			const auto components = str_t.starts_with("hsla") ? 4 : 3;
			const auto values = Trim(str_t.substr(components), "()");
			const auto tokens = Split(values, ',');
			if (tokens.size() != components) {
				return false;
			}

			float h, s, l, a;
			if (ParseFloat(tokens.at(0), h) && ParseFloat(tokens.at(1), s) && ParseFloat(tokens.at(2), l)) {
				if (components == 3) {
					a = 1.0f;
				} else {
					if (!ParseFloat<float>(tokens.at(3), a)) {
						return false;
					}
				}

				out = Color::FromHsl(h, s, l, a);
				return true;
			}
			return false;
		}
		if (str_t.starts_with('#')) {
			uint32_t rgba;
			if (!ParseInt(str_t, rgba)) {
				return false;
			}
			out = Color::FromRgba(rgba);
			return true;
		}
		return false;
	}
	bool ParseMatrix4(const std::string& str, Matrix4& out) {
		const auto tokens = Split(str, ' ');
		if (tokens.size() != 16) {
			return false;
		}

		for (int i = 0; i < 16; i++) {
			if (!ParseFloat(tokens.at(i), out.item(i))) {
				return false;
			}
		}
		return true;
	}

	bool ParseVector(const std::string& str, uint8_t item_count, std::vector<bool>& out) {
		std::string str_t = Trim(str);
		const auto items = Split(str, ' ');
		if (items.size() != item_count) {
			return false;
		}
		out.reserve(item_count);
		for (int i = 0; i < item_count; i++) {
			bool value;
			if (!ParseBool(items.at(i), value)) {
				return false;
			}
			out.push_back(value);
		}
		return true;
	}
}
