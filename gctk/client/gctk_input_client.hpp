#pragma once

#include "gctk_math.hpp"
#include "gctk_client.hpp"

#include <string>
#include <thread>

#define KEYMAP_VERSION 1

namespace gctk::Input {
	enum class KeyState : int8_t {
		Up,
		Pressed,
		Down,
		Released,

		Invalid = -1
	};

	namespace Modifiers {
		enum Type : uint8_t {
			None   = 0x00,
			LCtrl  = 0x01,
			LShift = 0x02,
			LAlt   = 0x04,
			LMeta  = 0x08,
			RCtrl  = 0x10,
			RShift = 0x20,
			RAlt   = 0x40,
			RMeta  = 0x80
		};
	}

	void Initialize(const Client& client);
	void Poll();

	bool CreateAxis(const std::string& name, const std::initializer_list<std::pair<std::string, std::string>>& pairs);
	bool CreateAxis(const std::string& name, std::vector<std::pair<std::string, std::string>>&& pairs);
	bool CreateAction(const std::string& name, const std::initializer_list<std::string>& keys);
	bool SetAxisMultiplier(const std::string& name, float multiplier);

	KeyState ActionState(const std::string& name);
	bool ActionPressed(const std::string& name);
	bool ActionPressedOrDown(const std::string& name);
	bool ActionDown(const std::string& name);
	bool ActionReleased(const std::string& name);
	bool ActionReleasedOrUp(const std::string& name);
	bool ActionUp(const std::string& name);
	float AxisValue(const std::string& name);
	Vector2 Vector2Value(const std::string& name_x, const std::string& name_y);
	Vector3 Vector3Value(const std::string& name_x, const std::string& name_y, const std::string& name_z);

	void SaveInputs();
}

template<>
struct std::formatter<gctk::Input::KeyState> : public std::formatter<std::string_view> {
	template<class FmtContext>
	typename FmtContext::iterator format(gctk::Input::KeyState value, FmtContext& ctx) const {
		std::string_view name;
		switch (value) {
			case gctk::Input::KeyState::Down: name = "Down"; break;
			case gctk::Input::KeyState::Up: name = "Up"; break;
			case gctk::Input::KeyState::Pressed: name = "Pressed"; break;
			case gctk::Input::KeyState::Released: name = "Released"; break;
			default: name = "Invalid"; break;
		}
		return std::formatter<std::string_view>::format(name, ctx);
	}
};