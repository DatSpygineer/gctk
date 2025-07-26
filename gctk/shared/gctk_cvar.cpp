#include "gctk_cvar.hpp"
#include "gctk_str.hpp"
#include "gctk_paths.hpp"

#include <memory>
#include <fstream>
#include <unordered_map>

namespace gctk {
#ifdef GCTK_SERVER
	CVar sv_cheats("sv_cheats", "false", CVAR_FLAG_REPLICATE);
#endif

	CONCOMMAND(exec, CVAR_DEFAULT_FLAGS) {
		if (!args.empty()) {
			Console::LoadConfig(args.at(0));
		}
	}

	CVar* CVar::s_cvars = nullptr;

	CVar* CVar::FindCVar(const std::string& name) {
		CVar* current = s_cvars;
		while (current != nullptr) {
			if (current->name() == name) {
				return current;
			}
			current = current->m_pNext;
		}
		return nullptr;
	}
	CVar* CVar::GetLastCvar() {
		CVar* previous = s_cvars;
		if (previous == nullptr) {
			return nullptr;
		}

		CVar* current = previous->m_pNext;
		while (current != nullptr) {
			previous = current;
			current = current->m_pNext;
		}
		return previous;
	}

	CVar::CVar(const std::string& name, const std::string& defaultValue, const int flags) :
		CVar(name, defaultValue, flags, ValidateAlwaysTrue) { }

	CVar::CVar(const std::string& name, std::string&& defaultValue, const int flags) :
		CVar(name, std::move(defaultValue), flags, ValidateAlwaysTrue) { }

	CVar::CVar(const std::string& name, const std::string& defaultValue, const int flags,
	           const ValidateCallback& validate) :
	m_sName(name), m_sValue(defaultValue), m_eFlags(flags | CVAR_DEFAULT_FLAGS), m_fnCallback(nullptr),
	m_fnValidate(validate), m_pNext(nullptr) {
		if (FindCVar(m_sName) != nullptr) {
			throw std::runtime_error("CVar \"" + name + "\" already exists");
		}

		if (CVar* last = GetLastCvar(); last == nullptr) {
			s_cvars = this;
		} else {
			last->m_pNext = this;
		}
	}

	CVar::CVar(const std::string& name, std::string&& defaultValue, const int flags,
	           const ValidateCallback& validate) :
	m_sName(name), m_sValue(std::move(defaultValue)), m_eFlags(flags | CVAR_DEFAULT_FLAGS), m_fnCallback(nullptr),
	m_fnValidate(validate), m_pNext(nullptr) {
		if (FindCVar(m_sName) != nullptr) {
			throw std::runtime_error("CVar \"" + m_sName + "\" already exists");
		}

		if (CVar* last = GetLastCvar(); last == nullptr) {
			s_cvars = this;
		} else {
			last->m_pNext = this;
		}
	}

	CVar::CVar(const std::string& name, const Callable& callable, const int flags) :
		m_sName(name), m_eFlags(flags | CVAR_DEFAULT_FLAGS), m_fnCallback(callable), m_pNext(nullptr) {
		if (FindCVar(m_sName) != nullptr) {
			throw std::runtime_error("CVar \"" + m_sName + "\" already exists");
		}

		if (CVar* last = GetLastCvar(); last == nullptr) {
			s_cvars = this;
		} else {
			last->m_pNext = this;
		}
	}

	const CVar::ValidateCallback CVar::ValidateAlwaysTrue = [](const CVar* self, const auto& value) -> bool {
		(void)self;
		(void)value;
		return true;
	};

	bool CVar::set_value(const bool value) {
		return set_value(std::string(value ? "true" : "false"));
	}
	bool CVar::set_value(const int value) {
		return set_value(std::to_string(value));
	}
	bool CVar::set_value(const float value) {
		return set_value(std::to_string(value));
	}

	bool CVar::set_value(const Vector2& value) {
		return set_value(std::format("{} {}", value.x, value.y));
	}
	bool CVar::set_value(const Vector3& value) {
		return set_value(std::format("{} {} {}", value.x, value.y, value.z));
	}
	bool CVar::set_value(const Vector4& value) {
		return set_value(std::format("{} {} {} {}", value.x, value.y, value.z, value.w));
	}
	bool CVar::set_value(const Color& value) {
		auto [ r, g, b, a ] = value.to_bytes();
		return set_value(std::format("{} {} {} {}", r, g, b, a));
	}

	bool CVar::set_value(const std::string& value) {
#ifdef GCTK_CLIENT
		if (!(m_eFlags & CVAR_FLAG_CLIENT_SIDE)) {
			return false;
		}
		// TODO: Ask server if cheats allowed
		if (m_eFlags & CVAR_FLAG_IS_CHEAT) {
			return false;
		}
#else
		if (!(m_eFlags & CVAR_FLAG_SERVER_SIDE)) {
			return false;
		}
		if ((m_eFlags & CVAR_FLAG_IS_CHEAT) && !sv_cheats.get_boolean()) {
			return false;
		}
#endif
		if (m_fnValidate != nullptr && !m_fnValidate(this, value)) {
			return false;
		}
		m_sValue = value;
		return true;
	}

	bool CVar::get_boolean() const {
		const auto lowercase = StringUtil::ToLower(m_sValue);
		if (lowercase == "true") {
			return true;
		}
		if (lowercase == "false") {
			return false;
		}
		return std::stoul(lowercase) != 0;
	}
	int CVar::get_integer() const {
		return std::stoi(m_sValue);
	}
	float CVar::get_float() const {
		return std::stof(m_sValue);
	}
	Vector2 CVar::get_vector2() const {
		const auto tokens = StringUtil::Split(m_sValue, ' ');
		if (tokens.size() != 2) {
			throw std::runtime_error("CVar::get_vector2: invalid number of tokens");
		}
		return Vector2 { std::stof(tokens[0]), std::stof(tokens[1]) };
	}
	Vector3 CVar::get_vector3() const {
		const auto tokens = StringUtil::Split(m_sValue, ' ');
		if (tokens.size() != 3) {
			throw std::runtime_error("CVar::get_vector3: invalid number of tokens");
		}
		return Vector3 { std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]) };
	}
	Vector4 CVar::get_vector4() const {
		const auto tokens = StringUtil::Split(m_sValue, ' ');
		if (tokens.size() != 4) {
			throw std::runtime_error("CVar::get_vector4: invalid number of tokens");
		}
		return Vector4 { std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) };
	}
	Color CVar::get_color() const {
		const auto tokens = StringUtil::Split(m_sValue, ' ');
		if (tokens.size() == 3 || tokens.size() == 4) {
			return Color::FromRgba(
				std::stoul(tokens[0]),
				std::stoul(tokens[1]),
				std::stoul(tokens[2]),
				tokens.size() == 4 ? std::stoul(tokens[3]) : 0xFF
			);
		}
		throw std::runtime_error("CVar::get_color: invalid number of tokens");
	}

	bool CVar::call(const std::vector<std::string>& args) const {
		if (m_fnCallback != nullptr) {
#ifdef GCTK_CLIENT
			if (!(m_eFlags & CVAR_FLAG_CLIENT_SIDE)) {
				return false;
			}
			// TODO: Ask server if cheats allowed
			if (m_eFlags & CVAR_FLAG_IS_CHEAT) {
				return false;
			}
#else
			if (!(m_eFlags & CVAR_FLAG_SERVER_SIDE)) {
				return false;
			}
			if ((m_eFlags & CVAR_FLAG_IS_CHEAT) && !sv_cheats.get_boolean()) {
				return false;
			}
#endif
			m_fnCallback(args);
			return true;
		}
		return false;
	}

	void Console::LoadConfig(const std::string& filename) {
		auto path = Paths::cfg_path() / filename;
		if (!path.has_extension()) {
			path += ".cfg";
		}

		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line)) {
			ExecuteCommand(line);
		}
	}
	bool Console::ExecuteCommand(const std::string& command) {
		std::string name, temp;
		std::vector<std::string> args;
		char quote = 0;
		bool escape = false;
		for (const auto& c : command) {
			if (temp.empty() && quote == 0) {
				if (c == '\"' || c == '\'') {
					quote = c;
				} else if (!std::isspace(c)) {
					temp.push_back(c);
				}
			} else {
				if (quote == 0) {
					if (std::isspace(c)) {
						if (name.empty()) {
							name = temp;
						} else {
							args.push_back(temp);
						}
						temp.clear();
					} else {
						temp.push_back(c);
					}
				} else {
					if (c == quote) {
						if (escape) {
							temp.push_back(c);
							escape = false;
						} else {
							if (name.empty()) {
								name = temp;
							} else {
								args.push_back(temp);
							}
							temp.clear();
							quote = 0;
						}
					} else if (c == '\\') {
						if (escape) {
							temp.push_back(c);
							escape = false;
						} else {
							escape = true;
						}
					} else {
						if (escape) {
							temp.push_back('\\');
							escape = false;
						}
						temp.push_back(c);
					}
 				}
			}
		}

		if (CVar::FindCVar(name) == nullptr) {
			return false;
		}

		const auto& cvar = CVar::FindCVar(name);
		if (cvar->is_callable()) {
			return cvar->call(args);
		}
		return cvar->set_value(StringUtil::Join(args, ' '));
	}

#ifdef GCTK_CLIENT
	bool Console::StoreUserData() {
		std::ofstream f(Paths::cfg_path() / "userdata.cfg");
		if (!f.is_open()) {
			return false;
		}

		const CVar* cvar = CVar::s_cvars;
		while (cvar != nullptr) {
			if (cvar->flags() & (CVAR_FLAG_CLIENT_SIDE | CVAR_FLAG_USER_DATA) && !cvar->is_callable()) {
				f << cvar->name() << " " << cvar->get_string() << std::endl;
			}
			cvar = cvar->m_pNext;
		}

		f.flush();
		f.close();
		return true;
	}
#endif
}
