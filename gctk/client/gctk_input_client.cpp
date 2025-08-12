#include "gctk_input_client.hpp"

#include <fstream>
#include <ranges>
#include <GLFW/glfw3.h>

#include "gctk_cvar.hpp"
#include "gctk_debug.hpp"
#include "gctk_filesys.hpp"
#include "gctk_str.hpp"

namespace gctk {
	CONCOMMAND(bind, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() >= 2, "Expected 2 or more arguments, got {}", args.size());
		Input::CreateAction(args.at(0), args.at(1));
	}
	CONCOMMAND(bind_axis, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() >= 3 && args.size() % 2 == 1, "Expected 3 or more arguments, got {}", args.size());
		Input::CreateAxis(args.at(0), args.at(1), args.at(2));
	}
	CONCOMMAND(bindmult, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() == 2, "Expected 2 arguments, got {}", args.size());

	}

	enum class InputType : uint8_t {
		Keyboard,
		MouseButton,
		MouseAxis,
		MouseWheel,
		GamepadButton,
		GamepadAxis
	};

	struct InputState {
		InputType type;
		uint8_t device_id;
		Input::Modifiers::Type modifiers;
		Input::KeyState keystate;

		uint32_t keycode;
		float value;
	};

	struct InputBinding {
		virtual float value() const = 0;
		virtual Input::KeyState keystate() const = 0;
	};

	struct ActionBinding : public InputBinding {
		std::vector<const InputState*> states;

		float value() const override {
			const auto state = keystate();
			return state == Input::KeyState::Down || state == Input::KeyState::Pressed ? 1.0f : 0.0f;
		}
		Input::KeyState keystate() const override {
			Input::KeyState last_state = Input::KeyState::Up;
			for (const auto state : states) {
				if (last_state == Input::KeyState::Down) {
					return last_state;
				}

				auto current_state = state->keystate;

				if (last_state == Input::keyState::Pressed && current_state != Input::keyState::Down) {
					return last_state;
				}
				last_state = current_state;
			}
			return last_state;
		}
	};
	struct AxisBinding : public InputBinding {
		std::vector<std::pair<const InputState*, const InputState*>> states;
		float multiplier = 1.0f;

		float value() const override {
			float value = 0.0f;
			for (const auto [ positive, negative ] : states) {
				value += positive.value - negative.value;
			}
			return value * multiplier;
		}
	};

	static std::unordered_map<std::string, InputState> s_input_map;
	static std::unordered_map<std::string, InputBinding> s_input_binds;

	static double s_mouse_x, s_mouse_y;
	static double s_mousewheel_x = 0, s_mousewheel_y = 0;

	static int StringToKeycode(const std::string& name);
	static std::string KeycodeToString(int key);
	static float GetKeyAsFloat(int key);

	void Input::Initialize(const Client& client) {
		glfwGetCursorPos(client.get_window(), &s_mouse_x, &s_mouse_y);
		glfwSetScrollCallback(client.get_window(), [](GLFWwindow* window, const double xoffset, const double yoffset) {
			(void)window;
			s_mousewheel_x = xoffset;
			s_mousewheel_y = yoffset;
		});
	}
	void Input::Poll() {
	}

	bool Input::CreateAxis(const std::string& name, const std::initializer_list<std::pair<std::string, std::string>>& pairs) {
		std::vector<std::pair<std::string, std::string>> inputs;
		for (const auto& pair : paris) {
			inputs.push_back(pair);
		}
		return CreateAxis(name, std::move(inputs));
	}
	bool CreateAxis(const std::string& name, std::vector<std::pair<std::string, std::string>>&& pairs) {
		if (s_input_binds.contains(name)) {
			return false;
		}

		for (const auto& [ positive, negative ] : pairs) {
			
		}

		return true;
	}
	bool Input::CreateAction(const std::string& name, const std::initializer_list<std::string>& keys) {

	}

	Input::KeyState Input::ActionState(const std::string& name) {
		if (s_input_binds.contains(name)) {
			return s_input_binds.at(name).keystate();
		}
		return Input::KeyState::Invalid;
	}
	bool Input::ActionPressed(const std::string& name) {
		return ActionState(name) == Input::KeyState::Pressed;
	}

	bool Input::ActionPressedOrDown(const std::string& name) {
		const auto state = ActionState(name);
		return state == Input::KeyState::Pressed || state == Input::KeyState::Down;
	}

	bool Input::ActionDown(const std::string& name) {
		return ActionState(name) == Input::KeyState::Down;
	}
	bool Input::ActionReleased(const std::string& name) {
		return ActionState(name) == Input::KeyState::Released;
	}

	bool Input::ActionReleasedOrUp(const std::string& name) {
		const auto state = ActionState(name);
		return state == Input::KeyState::Released || state == Input::KeyState::Up;
	}

	bool Input::ActionUp(const std::string& name) {
		return ActionState(name) == Input::KeyState::Up;
		
	}
	float Input::AxisValue(const std::string& name) {
		if (s_input_binds.contains(name)) {
			return s_input_binds.at(name).value();
		}
		return 0.0f;
	}
	Vector2 Input::Vector2Value(const std::string& name_x, const std::string& name_y) {
		return Vector2 { AxisValue(name_x), AxisValue(name_y) };
	}
	Vector3 Input::Vector3Value(const std::string& name_x, const std::string& name_y, const std::string& name_z) {
		return Vector3 { AxisValue(name_x), AxisValue(name_y), AxisValue(name_z) };
	}

	void Input::SaveInputs() {
		const auto path = Paths::CfgPath() / "keybinds.cfg";
		std::ofstream ofs(path);

		for (const auto& [name, keycode] : s_actions) {
			ofs << "bind " << name << " " << KeycodeToString(keycode) << std::endl;
		}

		ofs << std::endl;

		for (const auto& [name, keycodes] : s_axis) {
			const auto [ neg, pos ] = keycodes;
			ofs << "bind_axis " << name << " " << KeycodeToString(neg) << " " << KeycodeToString(pos) << std::endl;
		}

		ofs << std::endl;
		ofs.flush();
		ofs.close();
	}

	static int StringToKeycode(const std::string& name) {
		if (name.size() == 1) {
			const auto c = name[0];
			if (c >= '0' && c <= '9') {
				return c - '0' + GLFW_KEY_0;
			}
			if (c >= 'a' && c <= 'z') {
				return c - 'a' + GLFW_KEY_A;
			}
			if (c >= 'A' && c <= 'Z') {
				return c - 'a' + GLFW_KEY_A;
			}

			return GLFW_KEY_UNKNOWN;
		}

		const auto name_c = StringUtil::ToLower(name);
		if (name_c.starts_with("kp_")) {
			const auto c = name_c[3];
			if (c >= '0' && c <= '9') {
				return c - '0' + GLFW_KEY_KP_0;
			}
		}

		if (name_c[0] == 'f') {
			if (const auto i = std::stoi(name_c.substr(1)); i > 0 && i <= 25) {
				return GLFW_KEY_F1 + i;
			}
			return GLFW_KEY_UNKNOWN;
		}

		if (name_c == "space") {
			return GLFW_KEY_SPACE;
		}
		if (name_c == "apostrophe") {
			return GLFW_KEY_APOSTROPHE;
		}
		if (name_c == "comma") {
			return GLFW_KEY_COMMA;
		}
		if (name_c == "minus") {
			return GLFW_KEY_MINUS;
		}
		if (name_c == "period") {
			return GLFW_KEY_PERIOD;
		}
		if (name_c == "slash") {
			return GLFW_KEY_SLASH;
		}
		if (name_c == "semicolon") {
			return GLFW_KEY_SEMICOLON;
		}
		if (name_c == "equal") {
			return GLFW_KEY_EQUAL;
		}
		if (name_c == "left_bracket") {
			return GLFW_KEY_LEFT_BRACKET;
		}
		if (name_c == "backslash") {
			return GLFW_KEY_BACKSLASH;
		}
		if (name_c == "right_bracket") {
			return GLFW_KEY_RIGHT_BRACKET;
		}
		if (name_c == "grave_accent") {
			return GLFW_KEY_GRAVE_ACCENT;
		}
		if (name_c == "escape") {
			return GLFW_KEY_ESCAPE;
		}
		if (name_c == "enter" || name_c == "return") {
			return GLFW_KEY_ENTER;
		}
		if (name_c == "tab") {
			return GLFW_KEY_TAB;
		}
		if (name_c == "backspace") {
			return GLFW_KEY_BACKSPACE;
		}
		if (name_c == "insert") {
			return GLFW_KEY_INSERT;
		}
		if (name_c == "delete") {
			return GLFW_KEY_DELETE;
		}
		if (name_c == "right") {
			return GLFW_KEY_RIGHT;
		}
		if (name_c == "left") {
			return GLFW_KEY_LEFT;
		}
		if (name_c == "down") {
			return GLFW_KEY_DOWN;
		}
		if (name_c == "up") {
			return GLFW_KEY_UP;
		}
		if (name_c == "pageup") {
			return GLFW_KEY_PAGE_UP;
		}
		if (name_c == "pagedown") {
			return GLFW_KEY_PAGE_DOWN;
		}
		if (name_c == "home") {
			return GLFW_KEY_HOME;
		}
		if (name_c == "end") {
			return GLFW_KEY_END;
		}
		if (name_c == "capslock") {
			return GLFW_KEY_CAPS_LOCK;
		}
		if (name_c == "scroll_lock") {
			return GLFW_KEY_SCROLL_LOCK;
		}
		if (name_c == "numlock") {
			return GLFW_KEY_NUM_LOCK;
		}
		if (name_c == "printscreen") {
			return GLFW_KEY_PRINT_SCREEN;
		}
		if (name_c == "pause") {
			return GLFW_KEY_PAUSE;
		}
		if (name_c == "kp_decimal") {
			return GLFW_KEY_KP_DECIMAL;
		}
		if (name_c == "kp_divide") {
			return GLFW_KEY_KP_DIVIDE;
		}
		if (name_c == "kp_multiply") {
			return GLFW_KEY_KP_MULTIPLY;
		}
		if (name_c == "kp_subtract") {
			return GLFW_KEY_KP_SUBTRACT;
		}
		if (name_c == "kp_add") {
			return GLFW_KEY_KP_ADD;
		}
		if (name_c == "kp_enter") {
			return GLFW_KEY_KP_ENTER;
		}
		if (name_c == "kp_equal") {
			return GLFW_KEY_KP_EQUAL;
		}
		if (name_c == "lshift") {
			return GLFW_KEY_LEFT_SHIFT;
		}
		if (name_c == "lctrl") {
			return GLFW_KEY_LEFT_CONTROL;
		}
		if (name_c == "lalt") {
			return GLFW_KEY_LEFT_ALT;
		}
		if (name_c == "lmeta") {
			return GLFW_KEY_LEFT_SUPER;
		}
		if (name_c == "rshift") {
			return GLFW_KEY_RIGHT_SHIFT;
		}
		if (name_c == "rctrl") {
			return GLFW_KEY_RIGHT_CONTROL;
		}
		if (name_c == "ralt") {
			return GLFW_KEY_RIGHT_ALT;
		}
		if (name_c == "rmeta") {
			return GLFW_KEY_RIGHT_SUPER;
		}
		if (name_c == "menu") {
			return GLFW_KEY_MENU;
		}

		if (name_c.starts_with("mb")) {
			const auto n = name_c[2] - '0';
			if (n < GLFW_MOUSE_BUTTON_LAST) {
				return MOUSE_BUTTON_ORIGIN + n;
			}
			return GLFW_KEY_UNKNOWN;
		}

		if (name_c == "gamepad_1" || name_c == "gamepad_a" || name_c == "gamepad_cross") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_A;
		}
		if (name_c == "gamepad_2" || name_c == "gamepad_b" || name_c == "gamepad_circle") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_B;
		}
		if (name_c == "gamepad_3" || name_c == "gamepad_x" || name_c == "gamepad_square") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_X;
		}
		if (name_c == "gamepad_4" || name_c == "gamepad_y" || name_c == "gamepad_triangle") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_Y;
		}
		if (name_c == "gamepad_5" || name_c == "gamepad_lbumper") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
		}
		if (name_c == "gamepad_6" || name_c == "gamepad_rbumper") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
		}
		if (name_c == "gamepad_6" || name_c == "gamepad_back") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_BACK;
		}
		if (name_c == "gamepad_7" || name_c == "gamepad_start") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_START;
		}
		if (name_c == "gamepad_8" || name_c == "gamepad_guide") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_GUIDE;
		}
		if (name_c == "gamepad_9" || name_c == "gamepad_lthumb") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
		}
		if (name_c == "gamepad_10" || name_c == "gamepad_rthumb") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
		}
		if (name_c == "gamepad_11" || name_c == "gamepad_dpad_up") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_DPAD_UP;
		}
		if (name_c == "gamepad_12" || name_c == "gamepad_dpad_right") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
		}
		if (name_c == "gamepad_13" || name_c == "gamepad_dpad_down") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
		}
		if (name_c == "gamepad_14" || name_c == "gamepad_dpad_left") {
			return GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
		}

		if (name_c == "mwheel_x") {
			return MOUSE_WHEEL_ORIGIN;
		}
		if (name_c == "mwheel_y") {
			return MOUSE_WHEEL_ORIGIN + 1;
		}
		if (name_c == "mouse_x") {
			return MOUSE_MOTION_ORIGIN;
		}
		if (name_c == "mouse_y") {
			return MOUSE_MOTION_ORIGIN + 1;
		}

		if (name_c == "gamepad_axis_left_x") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_LEFT_X;
		}
		if (name_c == "gamepad_axis_left_y") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_LEFT_Y;
		}
		if (name_c == "gamepad_axis_left_trigger") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
		}
		if (name_c == "gamepad_axis_right_x") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_RIGHT_X;
		}
		if (name_c == "gamepad_axis_right_y") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_RIGHT_Y;
		}
		if (name_c == "gamepad_axis_right_trigger") {
			return GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
		}

		return GLFW_KEY_UNKNOWN;
	}

	static std::string KeycodeToString(const int key) {
		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			return std::format("{}", key - GLFW_KEY_0);
		}
		if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
			return std::format("kp_{}", key - GLFW_KEY_KP_0);
		}
		if (key >= GLFW_KEY_A && key <= GLFW_KEY_B) {
			return std::format("{}", 'A' + (key - GLFW_KEY_A));
		}
		if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F25) {
			return std::format("F{}", (key - GLFW_KEY_F1) + 1);
		}
		if (key >= GLFW_MOUSE_BUTTON_1 + MOUSE_BUTTON_ORIGIN && key <= GLFW_MOUSE_BUTTON_LAST + MOUSE_BUTTON_ORIGIN) {
			return std::format("mb{}", key - GLFW_MOUSE_BUTTON_1 + MOUSE_BUTTON_ORIGIN);
		}

		if (key >= MOUSE_WHEEL_ORIGIN && key < MOUSE_WHEEL_ORIGIN + 2) {
			if (key == MOUSE_WHEEL_ORIGIN) {
				return "mwheel_x";
			}
			return "mwheel_y";
		}
		if (key >= MOUSE_MOTION_ORIGIN && key < MOUSE_MOTION_ORIGIN + 2) {
			if (key == MOUSE_MOTION_ORIGIN) {
				return "mouse_x";
			}
			return "mouse_y";
		}

		if (key >= GAMEPAD_BUTTONS_ORIGIN && key < GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_LAST) {
			switch (key - GAMEPAD_BUTTONS_ORIGIN) {
				case GLFW_GAMEPAD_BUTTON_A: return "gamepad_a";
				case GLFW_GAMEPAD_BUTTON_B: return "gamepad_b";
				case GLFW_GAMEPAD_BUTTON_X: return "gamepad_x";
				case GLFW_GAMEPAD_BUTTON_Y: return "gamepad_y";
				case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return "gamepad_lbumper";
				case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return "gamepad_rbumper";
				case GLFW_GAMEPAD_BUTTON_BACK: return "gamepad_back";
				case GLFW_GAMEPAD_BUTTON_START: return "gamepad_start";
				case GLFW_GAMEPAD_BUTTON_GUIDE: return "gamepad_guide";
				case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return "gamepad_lthumb";
				case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return "gamepad_rthumb";
				case GLFW_GAMEPAD_BUTTON_DPAD_UP: return "gamepad_dpad_up";
				case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return "gamepad_dpad_right";
				case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return "gamepad_dpad_down";
				case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return "gamepad_dpad_left";
				default: return "unknown";
			}
		}

		if (key >= GAMEPAD_AXIS_ORIGIN && key < GAMEPAD_AXIS_ORIGIN + GLFW_GAMEPAD_AXIS_LAST) {
			switch (key - GAMEPAD_AXIS_ORIGIN) {
				case GLFW_GAMEPAD_AXIS_LEFT_X: return "gamepad_axis_left_x";
				case GLFW_GAMEPAD_AXIS_LEFT_Y: return "gamepad_axis_left_y";
				case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return "gamepad_axis_left_trigger";
				case GLFW_GAMEPAD_AXIS_RIGHT_X: return "gamepad_axis_right_x";
				case GLFW_GAMEPAD_AXIS_RIGHT_Y: return "gamepad_axis_right_y";
				case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return "gamepad_axis_right_trigger";
				default: return "unknown";
			}
		}

		switch (key) {
			case GLFW_KEY_SPACE: return "space";
			case GLFW_KEY_APOSTROPHE: return "apostrophe";
			case GLFW_KEY_COMMA: return "comma";
			case GLFW_KEY_MINUS: return "minus";
			case GLFW_KEY_PERIOD: return "period";
			case GLFW_KEY_SLASH: return "slash";
			case GLFW_KEY_SEMICOLON: return "semicolon";
			case GLFW_KEY_EQUAL: return "equal";
			case GLFW_KEY_LEFT_BRACKET: return "left_bracket";
			case GLFW_KEY_BACKSLASH: return "backslash";
			case GLFW_KEY_RIGHT_BRACKET: return "right_bracket";
			case GLFW_KEY_GRAVE_ACCENT: return "grave_accent";
			case GLFW_KEY_ESCAPE: return "escape";
			case GLFW_KEY_ENTER: return "enter";
			case GLFW_KEY_TAB: return "tab";
			case GLFW_KEY_BACKSPACE: return "backspace";
			case GLFW_KEY_INSERT: return "insert";
			case GLFW_KEY_DELETE: return "delete";
			case GLFW_KEY_RIGHT: return "right";
			case GLFW_KEY_LEFT: return "left";
			case GLFW_KEY_DOWN: return "down";
			case GLFW_KEY_UP: return "up";
			case GLFW_KEY_PAGE_UP: return "pageup";
			case GLFW_KEY_PAGE_DOWN: return "pagedown";
			case GLFW_KEY_HOME: return "home";
			case GLFW_KEY_END: return "end";
			case GLFW_KEY_CAPS_LOCK: return "capslock";
			case GLFW_KEY_SCROLL_LOCK: return "scroll_lock";
			case GLFW_KEY_NUM_LOCK: return "numlock";
			case GLFW_KEY_PRINT_SCREEN: return "printscreen";
			case GLFW_KEY_PAUSE: return "pause";
			case GLFW_KEY_KP_DECIMAL: return "kp_decimal";
			case GLFW_KEY_KP_DIVIDE: return "kp_divide";
			case GLFW_KEY_KP_MULTIPLY: return "kp_multiply";
			case GLFW_KEY_KP_SUBTRACT: return "kp_subtract";
			case GLFW_KEY_KP_ADD: return "kp_add";
			case GLFW_KEY_KP_ENTER: return "kp_enter";
			case GLFW_KEY_KP_EQUAL: return "kp_equal";
			case GLFW_KEY_LEFT_SHIFT: return "lshift";
			case GLFW_KEY_LEFT_CONTROL: return "lctrl";
			case GLFW_KEY_LEFT_ALT: return "lalt";
			case GLFW_KEY_LEFT_SUPER: return "lmeta";
			case GLFW_KEY_RIGHT_SHIFT: return "rshift";
			case GLFW_KEY_RIGHT_CONTROL: return "rctrl";
			case GLFW_KEY_RIGHT_ALT: return "ralt";
			case GLFW_KEY_RIGHT_SUPER: return "rmeta";
			case GLFW_KEY_MENU: return "menu";
			default: return "unknown";
		}
	}

	static float GetKeyAsFloat(const int key) {
		return NAN;
	}
}
