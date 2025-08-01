#pragma once

#include "gctk_math.hpp"
#include "gctk_client.hpp"

#include <string>
#include <thread>


namespace gctk::Input {
	enum class KeyState {
		Up,
		Pressed,
		Down,
		Released,

		Invalid = -1
	};

	void Initialize(const Client& client);
	void Poll();

	void CreateAxis(const std::string& name, const std::string& negative_key, const std::string& positive_key);
	void CreateAction(const std::string& name, const std::string& key);

	bool ActionPressed(const std::string& name);
	bool ActionPressedOrDown(const std::string& name);
	bool ActionDown(const std::string& name);
	bool ActionReleased(const std::string& name);
	bool ActionReleasedOrUp(const std::string& name);
	bool ActionUp(const std::string& name);
	float AxisValue(const std::string& name);
	Vector2 Vector2Value(const std::string& name_x, const std::string& name_y);
	Vector3 Vector3Value(const std::string& name_x, const std::string& name_y, const std::string& name_z);

	KeyState GetKeyState(const std::string& name);

	void SetTargetController(int index);
	int GetTargetController();

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