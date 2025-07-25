#pragma once

#include <functional>
#include <vector>

#include "gctk_math.hpp"

namespace gctk {
	enum CVarFlags {
		CVAR_FLAG_NONE        = 0x00,
		CVAR_FLAG_SERVER_SIDE = 0x01,
		CVAR_FLAG_CLIENT_SIDE = 0x02,
		CVAR_FLAG_REPLICATE   = 0x04,
		CVAR_FLAG_IS_CHEAT    = 0x08,
		CVAR_FLAG_USER_DATA   = 0x10
	};

	#ifdef GCTK_CLIENT
		#define CVAR_DEFAULT_FLAGS CVAR_FLAG_CLIENT_SIDE
	#else
		#define CVAR_DEFAULT_FLAGS CVAR_FLAG_SERVER_SIDE
	#endif

	class CVar {
	public:
		using ValidateCallback = std::function<bool(const CVar*, const std::string&)>;
		using Callable = std::function<void(const std::vector<std::string>&)>;
	private:
		std::string m_sName, m_sValue;
		int m_eFlags;
		Callable m_fnCallback;
		ValidateCallback m_fnValidate;

		CVar* m_pNext;
		static CVar* s_cvars;

		static CVar* FindCVar(const std::string& name);
		static CVar* GetLastCvar();
	public:
		CVar(const std::string& name, const std::string& defaultValue, int flags);
		CVar(const std::string& name, std::string&& defaultValue, int flags);
		CVar(const std::string& name, const std::string& defaultValue, int flags, const ValidateCallback& validate);
		CVar(const std::string& name, std::string&& defaultValue, int flags, const ValidateCallback& validate);
		CVar(const std::string& name, const Callable& callable, int flags);

		bool set_value(bool value);
		bool set_value(int value);
		bool set_value(float value);
		bool set_value(const Vector2& value);
		bool set_value(const Vector3& value);
		bool set_value(const Vector4& value);
		bool set_value(const Color& value);
		bool set_value(const std::string& value);

		[[nodiscard]] bool get_boolean() const;
		[[nodiscard]] int get_integer() const;
		[[nodiscard]] float get_float() const;
		[[nodiscard]] Vector2 get_vector2() const;
		[[nodiscard]] Vector3 get_vector3() const;
		[[nodiscard]] Vector4 get_vector4() const;
		[[nodiscard]] Color get_color() const;
		[[nodiscard]] constexpr const std::string& get_string() const { return m_sValue; }

		[[nodiscard]] bool call(const std::vector<std::string>& args) const;

		[[nodiscard]] constexpr const std::string& name() const { return m_sName; }
		[[nodiscard]] constexpr int flags() const { return m_eFlags; }
		[[nodiscard]] constexpr bool is_callable() const { return m_fnCallback != nullptr; }

		static const ValidateCallback ValidateAlwaysTrue;

		friend class Console;
	};

#define CONCOMMAND(__name, __flags) \
	void _cmd##__name(const std::vector<std::string>& args);\
	CVar __name(#__name, _cmd##__name, __flags);\
	void _cmd##__name(const std::vector<std::string>& args)

	class Console {
	public:
		static void LoadConfig(const std::string& filename);
		static bool ExecuteCommand(const std::string& command);
#ifdef GCTK_CLIENT
		static bool StoreUserData();
#endif
	};
}