#include "gctk_input_client.hpp"

#include <cstring>
#include <fstream>
#include <ranges>
#include <GLFW/glfw3.h>

#include "gctk_cvar.hpp"
#include "gctk_debug.hpp"
#include "gctk_filesys.hpp"
#include "gctk_str.hpp"

#define MOUSE_BUTTON_ORIGIN (GLFW_KEY_LAST + 1)
#define GAMEPAD_BUTTONS_ORIGIN (MOUSE_BUTTON_ORIGIN + 2)

#define MOUSE_WHEEL_ORIGIN  (GAMEPAD_BUTTONS_ORIGIN + GLFW_GAMEPAD_BUTTON_LAST + 1)
#define MOUSE_MOTION_ORIGIN (MOUSE_WHEEL_ORIGIN + 2)

#define GAMEPAD_AXIS_ORIGIN (MOUSE_MOTION_ORIGIN + 2)

namespace gctk {
	void _cmd_bind_mult(const std::vector<std::string>& args);

	CONCOMMAND(bind, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() >= 2, "Expected 2 or more arguments, got {}", args.size());
		std::vector<std::string> keys;
		keys.reserve(args.size() - 1);
		for (size_t i = 1; i < args.size(); ++i) {
			keys.push_back(args[i]);
		}
		Input::CreateAction(args.at(0), std::move(keys));
	}
	CONCOMMAND(bind_axis, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() >= 3 && args.size() % 2 == 1, "Expected 3 or more arguments, got {}", args.size());
		std::vector<std::pair<std::string, std::string>> pairs;

		for (size_t i = 1; i < args.size(); i += 2) {
			pairs.emplace_back(args[i], args[i + 1]);
		}

		Input::CreateAxis(args.at(0), std::move(pairs));
	}
	CVar bind_mult("bind_mult", &_cmd_bind_mult, CVAR_DEFAULT_FLAGS);

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

		int keycode;
		float value;
		int direction;
	};

	struct InputBinding {
		virtual ~InputBinding() = default;

		[[nodiscard]] virtual float value() const = 0;
		[[nodiscard]] virtual Input::KeyState keystate() const = 0;
		[[nodiscard]] virtual bool is_axis() const = 0;
	};

	struct ActionBinding final : public InputBinding {
		std::vector<InputState*> states;

		explicit ActionBinding(const std::vector<InputState*>& states) : states(states) { }
		explicit ActionBinding(std::vector<InputState*>&& states) : states(std::move(states)) { }

		[[nodiscard]] float value() const override {
			const auto state = keystate();
			return state == Input::KeyState::Down || state == Input::KeyState::Pressed ? 1.0f : 0.0f;
		}
		[[nodiscard]] Input::KeyState keystate() const override {
			Input::KeyState last_state = Input::KeyState::Up;
			for (const auto state : states) {
				if (last_state == Input::KeyState::Down) {
					return last_state;
				}

				auto current_state = state->keystate;

				if (last_state == Input::KeyState::Pressed && current_state != Input::KeyState::Down) {
					return last_state;
				}
				last_state = current_state;
			}
			return last_state;
		}
		[[nodiscard]] constexpr bool is_axis() const override {
			return false;
		}
	};
	struct AxisBinding final : public InputBinding {
		std::vector<std::pair<InputState*, InputState*>> states;
		float multiplier;

		explicit AxisBinding(const std::vector<std::pair<InputState*, InputState*>>& states, const float multiplier = 1.0f) :
			states(states), multiplier(multiplier) { }
		explicit AxisBinding(std::vector<std::pair<InputState*, InputState*>>&& states, const float multiplier = 1.0f) noexcept :
			states(std::move(states)), multiplier(multiplier) { }

		[[nodiscard]] float value() const override {
			float value = 0.0f;
			for (const auto [ negative, positive ] : states) {
				value += positive->value - negative->value;
			}
			return value * multiplier;
		}
		[[nodiscard]] constexpr Input::KeyState keystate() const override {
			return Input::KeyState::Invalid;
		}
		[[nodiscard]] constexpr bool is_axis() const override {
			return true;
		}
	};

	static std::unordered_map<std::string, InputState> s_input_map;
	static std::unordered_map<std::string, InputBinding*> s_input_binds;

	static double s_mouse_x, s_mouse_y;
	static double s_mousewheel_x = 0, s_mousewheel_y = 0;

	static int StringToKeycode(const std::string& name);
	static std::string KeycodeToString(int key);
	static InputType KeycodeToInputType(int key);

	void Input::Initialize(const Client& client) {
		glfwGetCursorPos(client.get_window(), &s_mouse_x, &s_mouse_y);
		glfwSetScrollCallback(client.get_window(), [](GLFWwindow* window, const double xoffset, const double yoffset) {
			(void)window;
			s_mousewheel_x = xoffset;
			s_mousewheel_y = yoffset;
		});

		if (Console::ConfigExists("keybinds.cfg")) {
			std::ifstream ifs(Paths::CfgPath() / "keybinds.cfg");

			std::string line;
			while (std::getline(ifs, line)) {
				line = StringUtil::Trim(line);
				if (line.starts_with("#keymap_version")) {
					line.erase(line.begin(), line.begin() + static_cast<std::string::difference_type>(strlen("#keymap_version")));
					if (uint32_t version; !StringUtil::ParseInt<uint32_t>(line, version)) {
						LogErr("Invalid version number \"{}\", expected a valid integer", line);
					} else if (version != KEYMAP_VERSION) {
						LogWarn("Input map uses version {}, current version is {}! Errors may occur", version, KEYMAP_VERSION);
					}
				}
			}
			ifs.close();

			Console::LoadConfig("keybinds.cfg");
		}
	}
	void Input::Poll() {
	}

	void Input::Dispose() {
		for (const auto& binding: s_input_binds | std::views::values) {
			delete binding;
		}
		s_input_binds.clear();
	}

	bool Input::CreateAxis(const std::string& name, const std::initializer_list<std::pair<std::string, std::string>>& pairs) {
		std::vector<std::pair<std::string, std::string>> inputs;
		for (const auto& pair : pairs) {
			inputs.push_back(pair);
		}
		return CreateAxis(name, std::move(inputs));
	}
	bool CreateAxis(const std::string& name, std::vector<std::pair<std::string, std::string>>&& pairs) {
		if (s_input_binds.contains(name)) {
			return false;
		}

		std::vector<std::pair<InputState*, InputState*>> states;

		for (auto [ negative, positive ] : pairs) {
			const auto pos = StringToKeycode(positive);
			if (pos == GLFW_KEY_UNKNOWN) {
				LogErr("Unknown key \"{}\"", positive);
				return false;
			}

			const auto neg = StringToKeycode(negative);
			if (neg == GLFW_KEY_UNKNOWN) {
				LogErr("Unknown key \"{}\"", negative);
				return false;
			}

			if (!s_input_map.contains(positive)) {
				s_input_map.emplace(positive, InputState {
					KeycodeToInputType(pos),
					0,
					Input::Modifiers::None,
					Input::KeyState::Up,
					pos,
					0.0f,
					positive.starts_with('+') ? 1 :
					(positive.starts_with('-') ? -1 : 0)
				});
			}
			if (!s_input_map.contains(negative)) {
				s_input_map.emplace(negative, InputState {
					KeycodeToInputType(neg),
					0,
					Input::Modifiers::None,
					Input::KeyState::Up,
					neg,
					0.0f,
					negative.starts_with('+') ? 1 :
					(negative.starts_with('-') ? -1 : 0)
				});
			}

			states.emplace_back(&s_input_map.at(positive), &s_input_map.at(negative));
		}

		s_input_binds.emplace(name, new AxisBinding { std::move(states) }); // NOLINT: Cleaned up by Input::Dispose

		return true; // NOLINT: Cleaned up by Input::Dispose
	}
	bool Input::CreateAction(const std::string& name, const std::initializer_list<std::string>& keys) {
		std::vector<std::string> keys_vec;
		keys_vec.reserve(keys.size());
		for (const auto& key : keys) {
			keys_vec.emplace_back(key);
		}
		return CreateAction(name, std::move(keys_vec));
	}
	bool Input::CreateAction(const std::string& name, std::vector<std::string>&& keys) {
		if (s_input_binds.contains(name)) {
			return false;
		}

		std::vector<InputState*> states;

		for (const auto& key : keys) {
			const auto code = StringToKeycode(key);
			if (code == GLFW_KEY_UNKNOWN) {
				LogErr("Unknown key \"{}\"", name);
				return false;
			}

			if (!s_input_map.contains(key)) {
				s_input_map.emplace(key, InputState {
					KeycodeToInputType(code),
					0,
					Input::Modifiers::None,
					Input::KeyState::Up,
					code,
					0.0f,
					key.starts_with('+') ? 1 :
					(key.starts_with('-') ? -1 : 0)
				});
			}

			states.emplace_back(&s_input_map.at(key));
		}

		s_input_binds.emplace(name, new ActionBinding { std::move(states) }); // NOLINT: Cleaned up by Input::Dispose

		return true; // NOLINT: Cleaned up by Input::Dispose
	}

	Input::KeyState Input::ActionState(const std::string& name) {
		if (s_input_binds.contains(name)) {
			return s_input_binds.at(name)->keystate();
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
			return s_input_binds.at(name)->value();
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
		ofs << "#keymap_version " << KEYMAP_VERSION << std::endl;

		for (const auto& [ name, binding ] : s_input_binds) {
			if (binding->is_axis()) {
				auto axis = dynamic_cast<const AxisBinding*>(binding);
				AssertThrow(axis != nullptr, "Failed to get axis binding data!");
				ofs << "bind_axis " << name;
				for (const auto& [ neg, pos ] : axis->states) {
					ofs << " " << KeycodeToString(neg->keycode) << " " << KeycodeToString(pos->keycode);
				}

				ofs << std::endl;

				if (axis->multiplier != 1.0f) {
					ofs << "bindmult " << name << " " << axis->multiplier << std::endl;
				}
			} else {
				auto action = dynamic_cast<const ActionBinding*>(binding);
				AssertThrow(action != nullptr, "Failed to get action binding data!");
				ofs << "bind " << name;
				for (const auto& key : action->states) {
					ofs << " " << KeycodeToString(key->keycode);
				}
				ofs << std::endl;
			}
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

		auto name_c = StringUtil::ToLower(name);
		if (name_c.starts_with('+') || name_c.starts_with('-')) {
			name_c = StringUtil::TrimBeg(name_c.substr(1));
		}

		if (name_c.starts_with("kp_")) {
			if (const auto c = name_c[3]; c >= '0' && c <= '9') {
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

	static InputType KeycodeToInputType(int key) {
		if (key < MOUSE_BUTTON_ORIGIN) {
			return InputType::Keyboard;
		}
		if (key < GAMEPAD_BUTTONS_ORIGIN) {
			return InputType::MouseButton;
		}
		if (key < MOUSE_WHEEL_ORIGIN) {
			return InputType::GamepadButton;
		}
		if (key < MOUSE_MOTION_ORIGIN) {
			return InputType::MouseWheel;
		}
		if (key < GAMEPAD_AXIS_ORIGIN) {
			return InputType::MouseAxis;
		}
		return InputType::GamepadAxis;
	}

	void _cmd_bind_mult(const std::vector<std::string>& args) {
		AssertThrow(args.size() >= 2, "Expected 2 or more arguments, got {}", args.size());
		const auto& name = args.at(0);

		if (!s_input_binds.contains(name)) {
			LogErr("Attempt to set axis multiplier for bind \"{}\", that doesn't exists!", name);
			return;
		}

		float value;
		if (!StringUtil::ParseFloat(args.at(1), value)) {
			LogErr("Expected a valid floating-point value for axis multiplier! Value \"{}\" is not a valid number", args.at(1));
			return;
		}

		if (s_input_binds.at(name)->is_axis()) {
			const auto axis = dynamic_cast<AxisBinding*>(s_input_binds.at(name));
			axis->multiplier = value;
		} else {
			LogWarn("Cannot set multiplier of action \"{}\"", name);
		}
	}
}
