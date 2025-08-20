#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "gctk_math.hpp"

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

	bool EqualsNoCase(const std::string& lhs, const std::string& rhs);
	bool Equals(const std::string& lhs, const std::string& rhs);

	bool ParseBool(const std::string& str, bool& out);
	template<IntegerType T>
	bool ParseInt(const std::string& str, T& out, uint8_t base = 10) {
		auto str_t = ToLower(Trim(str));
		if (str_t.starts_with("0b")) {
			base = 2;
			str_t = str_t.substr(2);
		} else if (str_t.starts_with("0o")) {
			base = 8;
			str_t = str_t.substr(2);
		} else if (str_t.starts_with("0x")) {
			base = 16;
			str_t = str_t.substr(2);
		} else if (str_t.starts_with("#")) {
			base = 16;
			str_t = str_t.substr(1);
		}

		try {
			if (std::is_signed_v<T>) {
				if (sizeof(T) <= sizeof(long)) {
					out = static_cast<T>(std::stol(str_t, nullptr, base));
				} else {
					out = static_cast<T>(std::stoll(str_t, nullptr, base));
				}
			} else {
				if (sizeof(T) <= sizeof(unsigned long)) {
					out = static_cast<T>(std::stoul(str_t, nullptr, base));
				} else {
					out = static_cast<T>(std::stoull(str_t, nullptr, base));
				}
			}
		} catch (...) {
			return false;
		}
		return true;
	}
	template<FloatType T>
	bool ParseFloat(const std::string& str, T& out) {
		auto str_t = Trim(str);
		bool percent = false;
		if (str_t.ends_with('%')) {
			percent = true;
			str_t = str_t.substr(0, str_t.size() - 1);
		}
		try {
			if (sizeof(T) <= sizeof(float)) {
				out = static_cast<T>(std::stof(str));
			} else if (sizeof(T) == sizeof(double)) {
				out = static_cast<T>(std::stod(str));
			} else {
				out = static_cast<T>(std::stold(str));
			}
			if (percent) {
				out /= 100.0;
			}
		} catch (...) {
			return false;
		}
		return true;
	}
	bool ParseVector2(const std::string& str, Vector2& out);
	bool ParseVector3(const std::string& str, Vector3& out);
	bool ParseVector4(const std::string& str, Vector4& out);
	bool ParseColor(const std::string& str, Color& out);
	bool ParseMatrix4(const std::string& str, Matrix4& out);
	template<FloatType T>
	bool ParseVector(const std::string& str, uint8_t item_count, std::vector<T>& out) {
		std::string str_t = Trim(str);
		const auto items = Split(str, ' ');
		if (items.size() != item_count) {
			return false;
		}
		out.reserve(item_count);
		for (int i = 0; i < item_count; i++) {
			T value;
			if (!ParseFloat<T>(items.at(i), value)) {
				return false;
			}
			out.push_back(value);
		}
		return true;
	}
	template<IntegerType T>
	bool ParseVector(const std::string& str, uint8_t item_count, std::vector<T>& out) {
		std::string str_t = Trim(str);
		const auto items = Split(str, ' ');
		if (items.size() != item_count) {
			return false;
		}
		out.reserve(item_count);
		for (int i = 0; i < item_count; i++) {
			T value;
			if (!ParseInt<T>(items.at(i), value)) {
				return false;
			}
			out.push_back(value);
		}
		return true;
	}
	bool ParseVector(const std::string& str, uint8_t item_count, std::vector<bool>& out);
}
