#include "gctk_input_client.hpp"

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
	CONCOMMAND(bind, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() == 2, "Expected 2 arguments, got {}", args.size());
		Input::CreateAction(args.at(0), args.at(1));
	}
	CONCOMMAND(bind_axis, CVAR_DEFAULT_FLAGS) {
		AssertThrow(args.size() == 3, "Expected 3 arguments, got {}", args.size());
		Input::CreateAxis(args.at(0), args.at(1), args.at(2));
	}

	static std::unordered_map<std::string, int> s_actions;
	static std::unordered_map<std::string, std::pair<int, int>> s_axis;
	static std::unordered_map<int, Input::KeyState> s_key_state;
	static std::unordered_map<int, float> s_axis_state;

	static double s_mouse_x, s_mouse_y;
	static double s_mousewheel_x = 0, s_mousewheel_y = 0;
	static int s_target_device = GLFW_JOYSTICK_1;

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
		for (const auto& keycode: s_actions | std::views::values) {
			if (!s_key_state.contains(keycode)) {
				s_key_state.emplace(keycode, KeyState::Up);
			}
			const auto state = s_key_state.at(keycode);

			if (keycode < MOUSE_BUTTON_ORIGIN) {
				const int current = glfwGetKey(Client::Instance()->get_window(), keycode);
				switch (state) {
					case KeyState::Up: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Pressed;
						}
					} break;
					case KeyState::Pressed: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Down;
						} else {
							s_key_state.at(keycode) = KeyState::Released;
						}
					} break;
					case KeyState::Down: {
						if (current == GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Released;
						}
					} break;
					case KeyState::Released: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Pressed;
						} else {
							s_key_state.at(keycode) = KeyState::Up;
						}
					} break;
					default: s_key_state.at(keycode) = KeyState::Up; break;
				}
			} else if (keycode < GAMEPAD_BUTTONS_ORIGIN) {
				const int current = glfwGetMouseButton(Client::Instance()->get_window(), keycode - GAMEPAD_BUTTONS_ORIGIN);
				switch (state) {
					case KeyState::Up: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Pressed;
						}
					} break;
					case KeyState::Pressed: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Down;
						} else {
							s_key_state.at(keycode) = KeyState::Released;
						}
					} break;
					case KeyState::Down: {
						if (current == GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Released;
						}
					} break;
					case KeyState::Released: {
						if (current != GLFW_RELEASE) {
							s_key_state.at(keycode) = KeyState::Pressed;
						} else {
							s_key_state.at(keycode) = KeyState::Up;
						}
					} break;
					default: s_key_state.at(keycode) = KeyState::Up; break;
				}
			} else if (keycode >= MOUSE_WHEEL_ORIGIN && keycode < MOUSE_MOTION_ORIGIN) {
				const double dt = (keycode == MOUSE_WHEEL_ORIGIN) ? s_mousewheel_x : s_mousewheel_y;
				if (s_axis_state.contains(keycode)) {
					s_axis_state.at(keycode) = static_cast<float>(dt);
				}
				if (s_key_state.contains(keycode)) {
					switch (s_key_state.at(keycode)) {
						case KeyState::Up: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Pressed;
							}
						} break;
						case KeyState::Pressed: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Down;
							} else {
								s_key_state.at(keycode) = KeyState::Released;
							}
						} break;
						case KeyState::Down: {
							if (dt == 0.0) {
								s_key_state.at(keycode) = KeyState::Released;
							}
						} break;
						case KeyState::Released: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Pressed;
							} else {
								s_key_state.at(keycode) = KeyState::Up;
							}
						} break;
						default: s_key_state.at(keycode) = KeyState::Up; break;
					}
				}
			} else if (keycode >= MOUSE_MOTION_ORIGIN && keycode < GAMEPAD_AXIS_ORIGIN) {
				double x, y;
				glfwGetCursorPos(Client::Instance()->get_window(), &x, &y);
				const double dt = (keycode == MOUSE_MOTION_ORIGIN) ? (x - s_mouse_x) : (y - s_mouse_y);
				if (s_axis_state.contains(keycode)) {
					s_axis_state.at(keycode) = static_cast<float>(dt);
				}
				if (s_key_state.contains(keycode)) {
					switch (s_key_state.at(keycode)) {
						case KeyState::Up: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Pressed;
							}
						} break;
						case KeyState::Pressed: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Down;
							} else {
								s_key_state.at(keycode) = KeyState::Released;
							}
						} break;
						case KeyState::Down: {
							if (dt == 0.0) {
								s_key_state.at(keycode) = KeyState::Released;
							}
						} break;
						case KeyState::Released: {
							if (dt != 0.0) {
								s_key_state.at(keycode) = KeyState::Pressed;
							} else {
								s_key_state.at(keycode) = KeyState::Up;
							}
						} break;
						default: s_key_state.at(keycode) = KeyState::Up; break;
					}
				}

				s_mouse_x = x;
				s_mouse_y = y;
			}
		}

		if (glfwJoystickIsGamepad(s_target_device)) {
			GLFWgamepadstate state;
			glfwGetGamepadState(s_target_device, &state);

			for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++) {
				const int key = GAMEPAD_BUTTONS_ORIGIN + i;
				if (s_key_state.contains(key)) {
					const int current = state.buttons[i];
					switch (s_key_state.at(key)) {
						case KeyState::Up: {
							if (current != GLFW_RELEASE) {
								s_key_state.at(key) = KeyState::Pressed;
							}
						} break;
						case KeyState::Pressed: {
							if (current != GLFW_RELEASE) {
								s_key_state.at(key) = KeyState::Down;
							} else {
								s_key_state.at(key) = KeyState::Released;
							}
						} break;
						case KeyState::Down: {
							if (current == GLFW_RELEASE) {
								s_key_state.at(key) = KeyState::Released;
							}
						} break;
						case KeyState::Released: {
							if (current != GLFW_RELEASE) {
								s_key_state.at(key) = KeyState::Pressed;
							} else {
								s_key_state.at(key) = KeyState::Up;
							}
						} break;
						default: s_key_state.at(key) = KeyState::Up; break;
					}
				}
			}
			for (int i = 0; i < GLFW_GAMEPAD_AXIS_LAST; i++) {
				const int key = i + GAMEPAD_AXIS_ORIGIN;
				if (s_axis_state.contains(key)) {
					s_axis_state.at(key) = state.axes[i];
				}
				if (s_key_state.contains(key)) {
					const double dt = state.axes[i];
					switch (s_key_state.at(key)) {
						case KeyState::Up: {
							if (dt != 0.0) {
								s_key_state.at(key) = KeyState::Pressed;
							}
						} break;
						case KeyState::Pressed: {
							if (dt != 0.0) {
								s_key_state.at(key) = KeyState::Down;
							} else {
								s_key_state.at(key) = KeyState::Released;
							}
						} break;
						case KeyState::Down: {
							if (dt == 0.0) {
								s_key_state.at(key) = KeyState::Released;
							}
						} break;
						case KeyState::Released: {
							if (dt != 0.0) {
								s_key_state.at(key) = KeyState::Pressed;
							} else {
								s_key_state.at(key) = KeyState::Up;
							}
						} break;
						default: s_key_state.at(key) = KeyState::Up; break;
					}
				}
			}
		}
	}

	void Input::CreateAxis(const std::string& name, const std::string& negative_key, const std::string& positive_key) {
		const int neg = StringToKeycode(negative_key);
		const int pos = StringToKeycode(positive_key);

		if (neg == GLFW_KEY_UNKNOWN) {
			LogErr("Failed to set input axis \"{}\": Negative key \"{}\" is invalid!", name, negative_key);
			return;
		}
		if (pos == GLFW_KEY_UNKNOWN) {
			LogErr("Failed to set input axis \"{}\": Positive key \"{}\" is invalid!", name, positive_key);
			return;
		}

		if (!s_key_state.contains(neg)) {
			s_key_state.emplace(neg, KeyState::Up);
		}
		if (!s_key_state.contains(pos)) {
			s_key_state.emplace(pos, KeyState::Up);
		}

		if (s_axis.contains(name)) {
			s_axis.at(name) = std::make_pair(neg, pos);
		} else {
			s_axis.emplace(name, std::make_pair(neg, pos));
		}
	}
	void Input::CreateAction(const std::string& name, const std::string& key) {
		const int code = StringToKeycode(key);
		if (code == GLFW_KEY_UNKNOWN) {
			LogErr("Failed to set input action \"{}\": Keycode \"{}\" is invalid!", name, key);
			return;
		}

		if (!s_key_state.contains(code)) {
			s_key_state.emplace(code, KeyState::Up);
		}

		if (s_actions.contains(name)) {
			s_actions.at(name) = code;
		} else {
			s_actions.emplace(name, code);
		}
	}

	bool Input::ActionPressed(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return false;
		}
		const auto keycode = s_actions.at(name);
		if (keycode == GLFW_KEY_UNKNOWN) {
			LogErr("Input action \"{}\" has no valid keycode assigned!", name);
			return false;
		}

		if (!s_key_state.contains(keycode)) {
			s_key_state.emplace(keycode, KeyState::Up);
			LogWarn("Keycode \"{}\" was not initialized! Try again...", KeycodeToString(keycode));
			return false;
		}

		return s_key_state.at(keycode) == KeyState::Pressed;
	}

	bool Input::ActionPressedOrDown(const std::string& name) {
		return ActionDown(name) || ActionPressed(name);
	}

	bool Input::ActionDown(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return false;
		}
		const auto keycode = s_actions.at(name);
		if (keycode == GLFW_KEY_UNKNOWN) {
			LogErr("Input action \"{}\" has no valid keycode assigned!", name);
			return false;
		}

		if (!s_key_state.contains(keycode)) {
			s_key_state.emplace(keycode, KeyState::Up);
			LogWarn("Keycode \"{}\" was not initialized! Try again...", KeycodeToString(keycode));
			return false;
		}

		return s_key_state.at(keycode) == KeyState::Down;
	}
	bool Input::ActionReleased(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return false;
		}
		const auto keycode = s_actions.at(name);
		if (keycode == GLFW_KEY_UNKNOWN) {
			LogErr("Input action \"{}\" has no valid keycode assigned!", name);
			return false;
		}

		if (!s_key_state.contains(keycode)) {
			s_key_state.emplace(keycode, KeyState::Up);
			LogWarn("Keycode \"{}\" was not initialized! Try again...", KeycodeToString(keycode));
			return false;
		}

		return s_key_state.at(keycode) == KeyState::Released;
	}

	bool Input::ActionReleasedOrUp(const std::string& name) {
		return ActionUp(name) || ActionReleased(name);
	}

	bool Input::ActionUp(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return false;
		}
		const auto keycode = s_actions.at(name);
		if (keycode == GLFW_KEY_UNKNOWN) {
			LogErr("Input action \"{}\" has no valid keycode assigned!", name);
			return false;
		}

		if (!s_key_state.contains(keycode)) {
			s_key_state.emplace(keycode, KeyState::Up);
			LogWarn("Keycode \"{}\" was not initialized! Try again...", KeycodeToString(keycode));
			return false;
		}

		return s_key_state.at(keycode) == KeyState::Up;
	}
	float Input::AxisValue(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return false;
		}
		const auto [ neg, pos ] = s_axis.at(name);
		if (neg == GLFW_KEY_UNKNOWN) {
			LogErr("Input axis \"{}\" has no valid negative keycode assigned!", name);
			return false;
		}
		if (pos == GLFW_KEY_UNKNOWN) {
			LogErr("Input axis \"{}\" has no valid positive keycode assigned!", name);
			return false;
		}

		const auto pos_val = GetKeyAsFloat(pos);
		const auto neg_val = GetKeyAsFloat(neg);

		if (pos_val == NAN) {
			LogErr("Could not get state of keycode \"{}\"", KeycodeToString(pos));
			return 0;
		}
		if (neg_val == NAN) {
			LogErr("Could not get state of keycode \"{}\"", KeycodeToString(neg));
			return 0;
		}

		return pos_val - neg_val;
	}
	Vector2 Input::Vector2Value(const std::string& name_x, const std::string& name_y) {
		return Vector2 { AxisValue(name_x), AxisValue(name_y) };
	}
	Vector3 Input::Vector3Value(const std::string& name_x, const std::string& name_y, const std::string& name_z) {
		return Vector3 { AxisValue(name_x), AxisValue(name_y), AxisValue(name_z) };
	}

	Input::KeyState Input::GetKeyState(const std::string& name) {
		if (!s_actions.contains(name)) {
			LogErr("Input action \"{}\" doesn't exists!", name);
			return KeyState::Invalid;
		}
		const auto keycode = s_actions.at(name);
		if (keycode == GLFW_KEY_UNKNOWN) {
			LogErr("Input action \"{}\" has no valid keycode assigned!", name);
			return KeyState::Invalid;
		}
		return s_key_state.at(keycode);
	}

	void Input::SetTargetController(const int index) {
		if (index > GLFW_JOYSTICK_LAST) {
			LogErr("Controller id {} is out of range! (Maximum device count is {})", index, GLFW_JOYSTICK_LAST);
			return;
		}

		s_target_device = index;
	}
	int Input::GetTargetController() {
		return s_target_device;
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
		if (s_key_state.contains(key)) {
			return s_key_state.at(key) == Input::KeyState::Up || s_key_state.at(key) == Input::KeyState::Pressed ? 1 : 0;
		}
		if (s_axis_state.contains(key)) {
			return s_axis_state.at(key);
		}
		return NAN;
	}
}
