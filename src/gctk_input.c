#include "gctk/input.h"
#include "gctk/debug.h"
#include "gctk/game.h"
#include "gctk/str.h"

#include <GLFW/glfw3.h>

typedef struct {
	hash_t hash;
	Input actions[64];
	size_t action_count;
	float value, prev_value;
} InputAction;

static InputAction GCTK_INPUT_MAP[GCTK_INPUT_MAP_MAX_COUNT] = { 0 };
static size_t GCTK_INPUT_MAP_COUNT = 0;

static Vec2 GCTK_MOUSE_POS;
static Vec2 GCTK_MOUSE_POS_PREV;
static Vec2 GCTK_MOUSE_WHEEL;

static void GctkMouseMovementCallback(GLFWwindow* window, double x, double y) {
	(void)window;
	GCTK_MOUSE_POS_PREV = GCTK_MOUSE_POS;
	GCTK_MOUSE_POS = VEC2((float)x, (float)y);

	Vec2 rel = GctkGetMousePositionRelative();
	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		InputAction* ia = GCTK_INPUT_MAP + i;
		float value = 0.0f;
		for (size_t j = 0; j < ia->action_count; j++) {
			if (ia->actions[j].device == GCTK_DEVICE_MOUSE_MOTION) {
				if (ia->actions[j].input == GCTK_INPUT_AXIS_X_PLUS) {
					value += rel.x;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_X_MINUS) {
					value -= rel.x;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_Y_PLUS) {
					value += rel.y;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_Y_MINUS) {
					value -= rel.y;
				}
			}
		}
		ia->prev_value = ia->value;
		ia->value = GctkClamp(value, -1.0f, 1.0f);
	}
}
static void GctkMouseScrollCallback(GLFWwindow* window, double x, double y) {
	(void)window;
	GCTK_MOUSE_WHEEL = VEC2((float)x, (float)y);

	Vec2 rel = GctkGetMousePositionRelative();
	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		InputAction* ia = GCTK_INPUT_MAP + i;
		float value = 0.0f;
		for (size_t j = 0; j < ia->action_count; j++) {
			if (ia->actions[j].device == GCTK_DEVICE_MOUSE_WHEEL) {
				if (ia->actions[j].input == GCTK_INPUT_AXIS_X_PLUS) {
					value += rel.x;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_X_MINUS) {
					value -= rel.x;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_Y_PLUS) {
					value += rel.y;
				} else if (ia->actions[j].input == GCTK_INPUT_AXIS_Y_MINUS) {
					value -= rel.y;
				}
			}
		}
		ia->prev_value = ia->value;
		ia->value = GctkClamp(value, -1.0f, 1.0f);
	}
}

Input GctkInputFromString(const char* input, int device_id) {
	if (GctkStrEqNoCase(input, "NONE")) {
		return INPUT_NONE;
	}

	if (GctkStrStartWithStrNoCase(input, "MB_")) {
		int mb;
		if (GctkStrEqNoCase(input, "MB_LEFT")) {
			mb = GLFW_MOUSE_BUTTON_LEFT;
		} else if (GctkStrEqNoCase(input, "MB_RIGHT")) {
			mb = GLFW_MOUSE_BUTTON_RIGHT;
		} else if (GctkStrEqNoCase(input, "MB_MIDDLE")) {
			mb = GLFW_MOUSE_BUTTON_MIDDLE;
		} else {
			if (!GctkStrParseToInt(input + 3, 10, &mb)) {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to parse string \"%s\", expected a valid integer after \"MB_\"", input);
				return INPUT_NONE;
			}
		}

		return (Input) {
			.input = mb,
			.device = GCTK_DEVICE_MOUSE_BUTTON,
			.device_id = device_id
		};
	}

	if (GctkStrEqNoCase(input, "MWHEEL_X+")) {
		return (Input) {
			.input = GCTK_INPUT_AXIS_X_PLUS,
			.device = GCTK_DEVICE_MOUSE_WHEEL,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "MWHEEL_X-")) {
		return (Input) {
			.input = GCTK_INPUT_AXIS_X_MINUS,
			.device = GCTK_DEVICE_MOUSE_WHEEL,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "MWHEEL_Y+") || GctkStrEqNoCase(input, "MWHEEL+")) {
		return (Input) {
			.input = GCTK_INPUT_AXIS_Y_PLUS,
			.device = GCTK_DEVICE_MOUSE_WHEEL,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "MWHEEL_Y-") || GctkStrEqNoCase(input, "MWHEEL-")) {
		return (Input) {
			.input = GCTK_INPUT_AXIS_Y_MINUS,
			.device = GCTK_DEVICE_MOUSE_WHEEL,
			.device_id = device_id
		};
	}

	if (GctkStrStartWithStrNoCase(input, "GPAD_")) {
		if (GctkStrStartWithStrNoCase(input, "GPAD_HAT_")) {
			int hat;
			if (GctkStrEqNoCase(input, "GPAD_HAT_UP")) {
				hat = GLFW_HAT_UP;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_RIGHT_UP")) {
				hat = GLFW_HAT_RIGHT_UP;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_RIGHT")) {
				hat = GLFW_HAT_RIGHT;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_RIGHT_DOWN")) {
				hat = GLFW_HAT_RIGHT_DOWN;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_DOWN")) {
				hat = GLFW_HAT_DOWN;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_LEFT_DOWN")) {
				hat = GLFW_HAT_LEFT_DOWN;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_LEFT")) {
				hat = GLFW_HAT_LEFT;
			} else if (GctkStrEqNoCase(input, "GPAD_HAT_LEFT_UP")) {
				hat = GLFW_HAT_LEFT_UP;
			} else {
				return INPUT_NONE;
			}

			return (Input) {
				.input = hat,
				.device = GCTK_DEVICE_MOUSE_BUTTON,
				.device_id = device_id
			};
		} else if (GctkStrStartWithStrNoCase(input, "GPAD_AXIS_")) {
			if (GctkStrEqNoCase(input, "GPAD_AXIS_LX")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_LEFT_X,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else if (GctkStrEqNoCase(input, "GPAD_AXIS_LY")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_LEFT_Y,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else if (GctkStrEqNoCase(input, "GPAD_AXIS_RX")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_RIGHT_X,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else if (GctkStrEqNoCase(input, "GPAD_AXIS_RY")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_RIGHT_Y,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else if (GctkStrEqNoCase(input, "GPAD_AXIS_LT")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else if (GctkStrEqNoCase(input, "GPAD_AXIS_RT")) {
				return (Input) {
					.input = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
					.device = GCTK_DEVICE_GAMEPAD_AXIS,
					.device_id = device_id
				};
			} else {
				int idx = strtol(input + strlen("GPAD_AXIS_"), NULL, 10);
				return (Input) {
						.input = idx,
						.device = GCTK_DEVICE_GAMEPAD_AXIS,
						.device_id = device_id
				};
			}
		} else if (GctkStrStartWithStrNoCase(input, "GPAD_BUTTON_")) {
			int button;
			if (GctkStrEqNoCase(input, "GPAD_BUTTON_A") || GctkStrEqNoCase(input, "GPAD_BUTTON_BUTTON_CROSS")) {
				button = GLFW_GAMEPAD_BUTTON_A;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_B") || GctkStrEqNoCase(input, "GPAD_BUTTON_BUTTON_CIRCLE")) {
				button = GLFW_GAMEPAD_BUTTON_B;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_X") || GctkStrEqNoCase(input, "GPAD_BUTTON_BUTTON_SQUARE")) {
				button = GLFW_GAMEPAD_BUTTON_X;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_Y") || GctkStrEqNoCase(input, "GPAD_BUTTON_BUTTON_TRIANGLE")) {
				button = GLFW_GAMEPAD_BUTTON_Y;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_BACK")) {
				button = GLFW_GAMEPAD_BUTTON_BACK;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_START")) {
				button = GLFW_GAMEPAD_BUTTON_START;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_GUIDE")) {
				button = GLFW_GAMEPAD_BUTTON_GUIDE;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_LB")) {
				button = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_RB")) {
				button = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_LT")) {
				button = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_RT")) {
				button = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_DPAD_UP")) {
				button = GLFW_GAMEPAD_BUTTON_DPAD_UP;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_DPAD_RIGHT")) {
				button = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_DPAD_DOWN")) {
				button = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
			} else if (GctkStrEqNoCase(input, "GPAD_BUTTON_DPAD_LEFT")) {
				button = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
			} else {
				button = strtol(input + strlen("GPAD_BUTTON_"), NULL, 10);
			}

			return (Input) {
				.input = button,
				.device = GCTK_DEVICE_GAMEPAD_BUTTON,
				.device_id = device_id
			};
		}
	}

	if ((input[0] == 'F' || input[0] == 'f') && input[1] >= '0' && input[1] <= '9' && strlen(input) == 2) {
		return (Input) {
				.input = strtol(input + 1, NULL, 10) + GLFW_KEY_F1,
				.device = GCTK_DEVICE_KEYBOARD,
				.device_id = device_id
		};
	}
	if (GctkStrStartWithStrNoCase(input, "KP_")) {
		return (Input) {
			.input = GLFW_KEY_KP_0 + strtol(input + strlen("KP_"), NULL, 10),
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (input[0] >= '0' && input[0] <= '9' && strlen(input) == 1) {
		return (Input) {
			.input = (input[0] - '0') + GLFW_KEY_0,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if ((input[0] >= 'a' && input[0] <= 'z') && strlen(input) == 1) {
		return (Input) {
			.input = (input[0] - 'a') + GLFW_KEY_A,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if ((input[0] >= 'A' && input[0] <= 'Z') && strlen(input) == 1) {
		return (Input) {
			.input = (input[0] - 'A') + GLFW_KEY_A,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}

	if (GctkStrEqNoCase(input, "SPACE")) {
		return (Input) {
			.input = GLFW_KEY_SPACE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "APOSTROPHE")) {
		return (Input) {
			.input = GLFW_KEY_APOSTROPHE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "COMMA")) {
		return (Input) {
			.input = GLFW_KEY_COMMA,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "MINUS")) {
		return (Input) {
			.input = GLFW_KEY_MINUS,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "PERIOD")) {
		return (Input) {
			.input = GLFW_KEY_PERIOD,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "SLASH")) {
		return (Input) {
			.input = GLFW_KEY_SLASH,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "SEMICOLON")) {
		return (Input) {
			.input = GLFW_KEY_SEMICOLON,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "EQUAL")) {
		return (Input) {
			.input = GLFW_KEY_EQUAL,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "LEFT_BRACKET")) {
		return (Input) {
			.input = GLFW_KEY_LEFT_BRACKET,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "BACKSLASH")) {
		return (Input) {
			.input = GLFW_KEY_BACKSLASH,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT_BRACKET")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT_BRACKET,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "GRAVE_ACCENT")) {
		return (Input) {
			.input = GLFW_KEY_GRAVE_ACCENT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "ESCAPE")) {
		return (Input) {
			.input = GLFW_KEY_ESCAPE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "ENTER") || GctkStrEqNoCase(input, "RETURN")) {
		return (Input) {
			.input = GLFW_KEY_ENTER,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "TAB")) {
		return (Input) {
			.input = GLFW_KEY_TAB,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "BACKSPACE")) {
		return (Input) {
			.input = GLFW_KEY_BACKSPACE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "INSERT")) {
		return (Input) {
			.input = GLFW_KEY_INSERT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "DELETE")) {
		return (Input) {
			.input = GLFW_KEY_DELETE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "LEFT")) {
		return (Input) {
			.input = GLFW_KEY_LEFT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "DOWN")) {
		return (Input) {
			.input = GLFW_KEY_DOWN,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "UP")) {
		return (Input) {
			.input = GLFW_KEY_UP,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "PAGE_UP")) {
		return (Input) {
			.input = GLFW_KEY_PAGE_UP,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "PAGE_DOWN")) {
		return (Input) {
			.input = GLFW_KEY_PAGE_DOWN,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "HOME")) {
		return (Input) {
			.input = GLFW_KEY_HOME,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "END")) {
		return (Input) {
			.input = GLFW_KEY_END,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "CAPS_LOCK")) {
		return (Input) {
			.input = GLFW_KEY_CAPS_LOCK,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "SCROLL_LOCK")) {
		return (Input) {
			.input = GLFW_KEY_SCROLL_LOCK,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "NUM_LOCK")) {
		return (Input) {
			.input = GLFW_KEY_NUM_LOCK,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "PRINT_SCREEN")) {
		return (Input) {
			.input = GLFW_KEY_PRINT_SCREEN,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "PAUSE")) {
		return (Input) {
			.input = GLFW_KEY_PAUSE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}

	if (GctkStrEqNoCase(input, "KP_DECIMAL")) {
		return (Input) {
			.input = GLFW_KEY_KP_DECIMAL,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_DIVIDE")) {
		return (Input) {
			.input = GLFW_KEY_KP_DIVIDE,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_MULTIPLY")) {
		return (Input) {
			.input = GLFW_KEY_KP_MULTIPLY,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_SUBTRACT")) {
		return (Input) {
			.input = GLFW_KEY_KP_SUBTRACT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_ADD")) {
		return (Input) {
			.input = GLFW_KEY_KP_ADD,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_ENTER")) {
		return (Input) {
			.input = GLFW_KEY_KP_ENTER,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "KP_EQUAL")) {
		return (Input) {
			.input = GLFW_KEY_KP_EQUAL,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}

	if (GctkStrEqNoCase(input, "LEFT_SHIFT")) {
		return (Input) {
			.input = GLFW_KEY_LEFT_SHIFT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "LEFT_CONTROL")) {
		return (Input) {
			.input = GLFW_KEY_LEFT_CONTROL,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "LEFT_ALT")) {
		return (Input) {
			.input = GLFW_KEY_LEFT_ALT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "LEFT_SUPER")) {
		return (Input) {
			.input = GLFW_KEY_LEFT_SUPER,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT_SHIFT")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT_SHIFT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT_CONTROL")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT_CONTROL,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT_ALT")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT_ALT,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "RIGHT_SUPER")) {
		return (Input) {
			.input = GLFW_KEY_RIGHT_SUPER,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (GctkStrEqNoCase(input, "MENU")) {
		return (Input) {
			.input = GLFW_KEY_MENU,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}

	return INPUT_NONE;
}

bool GctkLoadInputMap() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "inputs.cfg");

	FILE* f = fopen(path, "r");
	if (f == NULL) {
		return false;
	}

	char line[128] = { 0 };
	char key[64] = { 0 };
	Input action[64] = { 0 };
	size_t action_count = 0;
	int device_id = GCTK_GAMEPAD_ANY;
	while (GctkFileReadLine(f, line, 128, "\n")) {
		if (GctkStrStartWith(line, '[') && GctkStrEndWith(line, ']')) {
			if (strlen(key) > 0) {
				GctkSetInputAction(key, action_count, action);
				memset(key, 0, sizeof(char) * 64);
				memset(action, 0, sizeof(Input) * 64);
				action_count = 0;
			}

			ssize_t i = GctkStrFindLast(line, '|');
			if (i >= 0) {
				if (!GctkStrParseToInt(line + i + 1, 10, &device_id)) {
					GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to load input \"%s\": Failed to parse device id!", line);
					return false;
				}
			} else {
				i = GctkStrFindLast(line, ']');
			}
			GctkStrNCpy(key, line + 1, 64, i);
		} else {
			if (action_count >= 64) {
				GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to load input \"%s\": Input limit reached!", key);
				return false;
			}
			action[action_count++] = GctkInputFromString(line, device_id);
		}
	}

	return true;
}

void GctkSetupInputCallbacks() {
	GLFWwindow* win = (GLFWwindow*)GctkGetWindowHandle();
	glfwSetCursorPosCallback(win, &GctkMouseMovementCallback);
	glfwSetScrollCallback(win, &GctkMouseScrollCallback);
}

void GctkUpdateInputStates() {
	GLFWwindow* window = (GLFWwindow*)GctkGetWindowHandle();
	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		InputAction* ia = GCTK_INPUT_MAP + i;
		float state = 0.0f;
		bool handled = true;
		for (size_t j = 0; j < ia->action_count; j++) {
			switch (ia->actions[j].device) {
				case GCTK_DEVICE_KEYBOARD: {
					switch (glfwGetKey(window, ia->actions[j].input)) {
						case GLFW_PRESS:
						case GLFW_REPEAT: {
							state += 1.0f;
						} break;
						case GLFW_RELEASE: {
							state -= 1.0f;
						} break;
					}
					state = GctkClamp(state, 0.0f, 1.0f);
				} break;
				case GCTK_DEVICE_MOUSE_BUTTON: {
					switch (glfwGetMouseButton(window, ia->actions[j].input)) {
						case GLFW_PRESS:
						case GLFW_REPEAT: {
							state += 1.0f;
						} break;
						case GLFW_RELEASE: {
							state -= 1.0f;
						} break;
					}
					state = GctkClamp(state, 0.0f, 1.0f);
				} break;
				case GCTK_DEVICE_GAMEPAD_AXIS: {
					int axis_count;
					const float* axes = glfwGetJoystickAxes(ia->actions[j].device_id, &axis_count);
					state += (axes == NULL || axis_count < ia->actions[j].input) ? 0.0f : axes[ia->actions[j].input];
				} break;
				case GCTK_DEVICE_GAMEPAD_BUTTON: {
					int btn_count;
					const uint8_t* buttons = glfwGetJoystickButtons(ia->actions[j].device_id, &btn_count);
					state += (buttons == NULL || btn_count < ia->actions[j].input) ? 0.0f :
							(buttons[ia->actions[j].input] == GLFW_PRESS ? 1.0f : 0.0f);
				} break;
				case GCTK_DEVICE_GAMEPAD_HAT: {
					// TODO: Implement this!
				} break;
				default: handled = false; /* Not handled here */ break;
			}
		}

		if (handled) {
			ia->prev_value = ia->value;
			ia->value = state;
		}
	}
}

Vec2 GctkGetMousePosition() {
	return GCTK_MOUSE_POS;
}
Vec2 GctkGetMousePositionRelative() {
	return Vec2Sub(GCTK_MOUSE_POS, GCTK_MOUSE_POS_PREV);
}
Vec2 GctkGetMouseWheel() {
	return GCTK_MOUSE_WHEEL;
}

bool GctkSetInputAction(const char* action, size_t input_count, const Input* inputs) {
	if (input_count > 64) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to set input action \"%s\": Too many input entries!", action);
		return false;
	}

	hash_t hash = GctkStrHash(action);
	bool found = false;
	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		if (GCTK_INPUT_MAP[i].hash == hash) {
			GCTK_INPUT_MAP[i] = (InputAction) {
				.hash = hash,
				.value = 0.0f,
				.prev_value = 0.0f,
				.action_count = input_count
			};
			memset(GCTK_INPUT_MAP[i].actions, 0, sizeof(Input) * 64);
			memcpy(GCTK_INPUT_MAP[i].actions, inputs, sizeof(Input) * input_count);
			found = true;
		}
	}

	if (!found) {
		if (GCTK_INPUT_MAP_COUNT == GCTK_INPUT_MAP_MAX_COUNT) {
			GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to set input action \"%s\": Input map is full!", action);
			return false;
		}
		GCTK_INPUT_MAP[GCTK_INPUT_MAP_COUNT] = (InputAction) {
			.hash = hash,
			.value = 0.0f,
			.prev_value = 0.0f,
			.action_count = input_count
		};
		memset(GCTK_INPUT_MAP[GCTK_INPUT_MAP_COUNT].actions, 0, sizeof(Input) * 64);
		memcpy(GCTK_INPUT_MAP[GCTK_INPUT_MAP_COUNT].actions, inputs, sizeof(Input) * input_count);
		GCTK_INPUT_MAP_COUNT++;
	}

	return true;
}
bool GctkSetInputActionVA(const char* action, size_t input_count, ...) {
	Input inputs[64] = { 0 };

	if (input_count > 64) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to set input action \"%s\": Too many input entries!", action);
		return false;
	}

	va_list args;
	va_start(args, input_count);
	for (size_t i = 0; i < input_count; i++) {
		inputs[i] = va_arg(args, Input);
	}
	va_end(args);

	return GctkSetInputAction(action, input_count, inputs);
}
bool GctkInputActionDefined(const char* action) {
	hash_t hash = GctkStrHash(action);

	for (int i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		if (GCTK_INPUT_MAP[i].hash == hash) {
			return true;
		}
	}
	return false;
}

static bool GctkInputGetAction(const char* name, InputAction* action) {
	hash_t hash = GctkStrHash(name);
	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		if (GCTK_INPUT_MAP[i].hash == hash) {
			*action = GCTK_INPUT_MAP[i];
			return true;
		}
	}
	return false;
}

InputState GctkInputActionState(const char* action) {
	InputAction ia;
	if (GctkInputGetAction(action, &ia)) {
		if (ia.value != 0.0) {
			if (ia.prev_value != 0.0) {
				return GCTK_STATE_DOWN;
			} else {
				return GCTK_STATE_PRESSED;
			}
		} else {
			if (ia.prev_value != 0.0) {
				return GCTK_STATE_RELEASED;
			} else {
				return GCTK_STATE_UP;
			}
		}
	}
	return GCTK_STATE_UNDEFINED;
}
float GctkInputGetValue(const char* action) {
	InputAction ia;
	if (GctkInputGetAction(action, &ia)) {
		return ia.value;
	}
	return 0.0f;
}

float GctkInputGetAxis(const char* negative_action, const char* positive_action) {
	return GctkInputGetValue(positive_action) - GctkInputGetValue(negative_action);
}
Vec2 GctkInputGetVector(
		const char* negative_x_action, const char* positive_x_action,
		const char* negative_y_action, const char* positive_y_action
) {
	return VEC2(
		GctkInputGetAxis(negative_x_action, positive_x_action),
		GctkInputGetAxis(negative_y_action, positive_y_action)
	);
}