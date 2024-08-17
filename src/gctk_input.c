#include "gctk/input.h"
#include "gctk/debug.h"
#include "gctk/game.h"
#include "gctk/str.h"

#include <GLFW/glfw3.h>

typedef struct {
	hash_t hash;
	char name[128];
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

	if (strlen(input) == 2 && (input[0] == 'F' || input[0] == 'f') && input[1] >= '0' && input[1] <= '9') {
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
	if (strlen(input) == 1 && (input[0] >= 'a' && input[0] <= 'z')) {
		return (Input) {
			.input = (input[0] - 'a') + GLFW_KEY_A,
			.device = GCTK_DEVICE_KEYBOARD,
			.device_id = device_id
		};
	}
	if (strlen(input) == 1 && (input[0] >= 'A' && input[0] <= 'Z')) {
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

bool GctkInputMapExists() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "inputs.cfg");
	return GctkPathExists(path);
}

bool GctkLoadInputMap() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "inputs.cfg");

	FILE* f = GctkOpenFile(path, GCTK_FILEMODE_READ, GCTK_FILE_OPEN);
	if (f == NULL) {
		GctkLogWarn("Couldn't find input map \"%s\"", path);
		return false;
	}

	char line[128] = { 0 };
	char key[64] = { 0 };
	Input action[64] = { 0 };
	size_t action_count = 0;
	int device_id = GCTK_GAMEPAD_ANY;
	while (GctkFileReadLine(f, line, 128, "\n")) {
		ssize_t j = GctkStrFindAnyLastNot(line, " \t\n\r");
		line[j + 1] = '\0';
		j = GctkStrFindAnyNot(line, " \t\n\r");

		const char* line_trimmed = line + j;

		if (GctkStrStartWith(line_trimmed, '[') && GctkStrEndWith(line_trimmed, ']')) {
			if (strlen(key) > 0) {
				GctkSetInputAction(key, action_count, action);
				memset(key, 0, sizeof(char) * 64);
				memset(action, 0, sizeof(Input) * 64);
				action_count = 0;
			}

			ssize_t i = GctkStrFindLast(line_trimmed, '|');
			if (i >= 0) {
				if (!GctkStrParseToInt(line_trimmed + i + 1, 10, &device_id)) {
					GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to load input \"%s\": Failed to parse device id!", line_trimmed);
					return false;
				}
			} else {
				i = GctkStrFindLast(line_trimmed, ']');
			}
			GctkStrCpySlice(key, line_trimmed + 1, 64, i - 1);
		} else {
			if (action_count >= 64) {
				GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to load input \"%s\": Input limit reached!", key);
				return false;
			}
			Input input = GctkInputFromString(line, device_id);
			if (INPUT_IS_NONE(input)) {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to parse input \"%s\"!", line_trimmed);
				continue;
			}
			action[action_count++] = input;
		}
	}
	if (strlen(key) > 0) {
		GctkSetInputAction(key, action_count, action);
	}

	return true;
}

bool GctkWriteInputMap() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "inputs.cfg");

	FILE* f = GctkOpenFile(path, GCTK_FILEMODE_WRITE, GCTK_FILE_OPEN_OR_CREATE);
	if (f == NULL) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Failed to open/create input map file \"%s\"", path);
		return false;
	}

	for (size_t i = 0; i < GCTK_INPUT_MAP_COUNT; i++) {
		fprintf(f, "[%s]\n", GCTK_INPUT_MAP[i].name);
		for (size_t j = 0; j < GCTK_INPUT_MAP[i].action_count; j++) {
			char input_name[256] = { 0 };
			GctkInputToString(input_name, 256, GCTK_INPUT_MAP[i].actions[j]);
			fprintf(f, "%s\n", input_name);
		}
	}

	GctkFileWrite_str(f, "\n");
	GctkFlushFile(f);
	GctkCloseFile(f);

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

char* GctkInputToString(char* buffer, size_t buffer_size, Input input) {
	switch (input.device) {
		case GCTK_DEVICE_NONE: return NULL; break;
		case GCTK_DEVICE_KEYBOARD: {
			if (input.input >= GLFW_KEY_0 && input.input <= GLFW_KEY_9) {
				snprintf(buffer, buffer_size, "KP_%d", input.input);
				return buffer;
			}

			if (input.input >= GLFW_KEY_A && input.input <= GLFW_KEY_Z) {
				snprintf(buffer, buffer_size, "%c", (input.input - GLFW_KEY_A) + 'A');
				return buffer;
			}

			switch (input.input) {
				case GLFW_KEY_SPACE: snprintf(buffer, buffer_size, "SPACE"); return buffer;
				case GLFW_KEY_APOSTROPHE: snprintf(buffer, buffer_size, "APOSTROPHE"); return buffer;
				case GLFW_KEY_COMMA: snprintf(buffer, buffer_size, "COMMA"); return buffer;
				case GLFW_KEY_MINUS: snprintf(buffer, buffer_size, "MINUS"); return buffer;
				case GLFW_KEY_PERIOD: snprintf(buffer, buffer_size, "PERIOD"); return buffer;
				case GLFW_KEY_SLASH: snprintf(buffer, buffer_size, "SLASH"); return buffer;
				case GLFW_KEY_SEMICOLON: snprintf(buffer, buffer_size, "SEMICOLON"); return buffer;
				case GLFW_KEY_EQUAL: snprintf(buffer, buffer_size, "EQUAL"); return buffer;
				case GLFW_KEY_LEFT_BRACKET: snprintf(buffer, buffer_size, "LEFT_BRACKET"); return buffer;
				case GLFW_KEY_BACKSLASH: snprintf(buffer, buffer_size, "BACKSLASH"); return buffer;
				case GLFW_KEY_RIGHT_BRACKET: snprintf(buffer, buffer_size, "RIGHT_BRACKET"); return buffer;
				case GLFW_KEY_GRAVE_ACCENT: snprintf(buffer, buffer_size, "GRAVE_ACCENT"); return buffer;
				case GLFW_KEY_ESCAPE: snprintf(buffer, buffer_size, "ESCAPE"); return buffer;
				case GLFW_KEY_ENTER: snprintf(buffer, buffer_size, "ENTER"); return buffer;
				case GLFW_KEY_TAB: snprintf(buffer, buffer_size, "TAB"); return buffer;
				case GLFW_KEY_BACKSPACE: snprintf(buffer, buffer_size, "BACKSPACE"); return buffer;
				case GLFW_KEY_INSERT: snprintf(buffer, buffer_size, "INSERT"); return buffer;
				case GLFW_KEY_DELETE: snprintf(buffer, buffer_size, "DELETE"); return buffer;
				case GLFW_KEY_RIGHT: snprintf(buffer, buffer_size, "RIGHT"); return buffer;
				case GLFW_KEY_LEFT: snprintf(buffer, buffer_size, "LEFT"); return buffer;
				case GLFW_KEY_UP: snprintf(buffer, buffer_size, "UP"); return buffer;
				case GLFW_KEY_DOWN: snprintf(buffer, buffer_size, "DOWN"); return buffer;
				case GLFW_KEY_PAGE_UP: snprintf(buffer, buffer_size, "PAGE_UP"); return buffer;
				case GLFW_KEY_PAGE_DOWN: snprintf(buffer, buffer_size, "PAGE_DOWN"); return buffer;
				case GLFW_KEY_HOME: snprintf(buffer, buffer_size, "HOME"); return buffer;
				case GLFW_KEY_END: snprintf(buffer, buffer_size, "END"); return buffer;
				case GLFW_KEY_CAPS_LOCK: snprintf(buffer, buffer_size, "CAPS_LOCK"); return buffer;
				case GLFW_KEY_SCROLL_LOCK: snprintf(buffer, buffer_size, "SCROLL_LOCK"); return buffer;
				case GLFW_KEY_NUM_LOCK: snprintf(buffer, buffer_size, "NUM_LOCK"); return buffer;
				case GLFW_KEY_PRINT_SCREEN: snprintf(buffer, buffer_size, "PRINT_SCREEN"); return buffer;
				case GLFW_KEY_PAUSE: snprintf(buffer, buffer_size, "PAUSE"); return buffer;
				case GLFW_KEY_KP_DECIMAL: snprintf(buffer, buffer_size, "KP_DECIMAL"); return buffer;
				case GLFW_KEY_KP_DIVIDE: snprintf(buffer, buffer_size, "KP_DIVIDE"); return buffer;
				case GLFW_KEY_KP_MULTIPLY: snprintf(buffer, buffer_size, "KP_MULTIPLY"); return buffer;
				case GLFW_KEY_KP_SUBTRACT: snprintf(buffer, buffer_size, "KP_SUBTRACT"); return buffer;
				case GLFW_KEY_KP_ADD: snprintf(buffer, buffer_size, "KP_ADD"); return buffer;
				case GLFW_KEY_KP_ENTER: snprintf(buffer, buffer_size, "KP_ENTER"); return buffer;
				case GLFW_KEY_KP_EQUAL: snprintf(buffer, buffer_size, "KP_EQUAL"); return buffer;
				case GLFW_KEY_LEFT_SHIFT: snprintf(buffer, buffer_size, "LEFT_SHIFT"); return buffer;
				case GLFW_KEY_LEFT_CONTROL: snprintf(buffer, buffer_size, "LEFT_CONTROL"); return buffer;
				case GLFW_KEY_LEFT_ALT: snprintf(buffer, buffer_size, "LEFT_ALT"); return buffer;
				case GLFW_KEY_LEFT_SUPER: snprintf(buffer, buffer_size, "LEFT_SUPER"); return buffer;
				case GLFW_KEY_RIGHT_SHIFT: snprintf(buffer, buffer_size, "RIGHT_SHIFT"); return buffer;
				case GLFW_KEY_RIGHT_CONTROL: snprintf(buffer, buffer_size, "RIGHT_CONTROL"); return buffer;
				case GLFW_KEY_RIGHT_ALT: snprintf(buffer, buffer_size, "RIGHT_ALT"); return buffer;
				case GLFW_KEY_RIGHT_SUPER: snprintf(buffer, buffer_size, "RIGHT_SUPER"); return buffer;
				case GLFW_KEY_MENU: snprintf(buffer, buffer_size, "MENU"); return buffer;
				default: return NULL;
			}
		}
		case GCTK_DEVICE_MOUSE_BUTTON: {
			switch (input.input) {
				case 0: snprintf(buffer, buffer_size, "MB_LEFT"); return buffer;
				case 1: snprintf(buffer, buffer_size, "MB_RIGHT"); return buffer;
				case 2: snprintf(buffer, buffer_size, "MB_MIDDLE"); return buffer;
				default: snprintf(buffer, buffer_size, "MB_%d", input.input); return buffer;
			}
		}
		case GCTK_DEVICE_MOUSE_WHEEL: {
			switch (input.input) {
				case GCTK_INPUT_AXIS_X_PLUS: snprintf(buffer, buffer_size, "MWHEEL_X+"); return buffer;
				case GCTK_INPUT_AXIS_X_MINUS: snprintf(buffer, buffer_size, "MWHEEL_X-"); return buffer;
				case GCTK_INPUT_AXIS_Y_PLUS: snprintf(buffer, buffer_size, "MWHEEL+"); return buffer;
				case GCTK_INPUT_AXIS_Y_MINUS: snprintf(buffer, buffer_size, "MWHEEL-"); return buffer;
				default: return NULL;
			}
		}
		case GCTK_DEVICE_MOUSE_MOTION: {
			switch (input.input) {
				case GCTK_MOUSE_X_PLUS: snprintf(buffer, buffer_size, "MOUSE_X+"); return buffer;
				case GCTK_MOUSE_X_MINUS: snprintf(buffer, buffer_size, "MOUSE_X-"); return buffer;
				case GCTK_MOUSE_Y_PLUS: snprintf(buffer, buffer_size, "MOUSE_Y+"); return buffer;
				case GCTK_MOUSE_Y_MINUS: snprintf(buffer, buffer_size, "MOUSE_Y-"); return buffer;
				default: return NULL;
			}
		}
		case GCTK_DEVICE_GAMEPAD_BUTTON: {
			switch (input.input) {
				case GLFW_GAMEPAD_BUTTON_A: snprintf(buffer, buffer_size, "GPAD_BUTTON_A"); return buffer;
				case GLFW_GAMEPAD_BUTTON_B: snprintf(buffer, buffer_size, "GPAD_BUTTON_B"); return buffer;
				case GLFW_GAMEPAD_BUTTON_X: snprintf(buffer, buffer_size, "GPAD_BUTTON_X"); return buffer;
				case GLFW_GAMEPAD_BUTTON_Y: snprintf(buffer, buffer_size, "GPAD_BUTTON_Y"); return buffer;
				case GLFW_GAMEPAD_BUTTON_BACK: snprintf(buffer, buffer_size, "GPAD_BUTTON_BACK"); return buffer;
				case GLFW_GAMEPAD_BUTTON_START: snprintf(buffer, buffer_size, "GPAD_BUTTON_START"); return buffer;
				case GLFW_GAMEPAD_BUTTON_GUIDE: snprintf(buffer, buffer_size, "GPAD_BUTTON_GUIDE"); return buffer;
				case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: snprintf(buffer, buffer_size, "GPAD_BUTTON_LB"); return buffer;
				case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: snprintf(buffer, buffer_size, "GPAD_BUTTON_RB"); return buffer;
				case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: snprintf(buffer, buffer_size, "GPAD_BUTTON_LT"); return buffer;
				case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: snprintf(buffer, buffer_size, "GPAD_BUTTON_RT"); return buffer;
				case GLFW_GAMEPAD_BUTTON_DPAD_UP: snprintf(buffer, buffer_size, "GPAD_BUTTON_DPAD_UP"); return buffer;
				case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: snprintf(buffer, buffer_size, "GPAD_BUTTON_DPAD_DOWN"); return buffer;
				case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: snprintf(buffer, buffer_size, "GPAD_BUTTON_DPAD_LEFT"); return buffer;
				case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: snprintf(buffer, buffer_size, "GPAD_BUTTON_DPAD_RIGHT"); return buffer;
				default: return NULL;
			}
		}
		case GCTK_DEVICE_GAMEPAD_AXIS: {
			switch (input.input) {
				case GLFW_GAMEPAD_AXIS_LEFT_X: snprintf(buffer, buffer_size, "GPAD_AXIS_LX"); return buffer;
				case GLFW_GAMEPAD_AXIS_LEFT_Y: snprintf(buffer, buffer_size, "GPAD_AXIS_LY"); return buffer;
				case GLFW_GAMEPAD_AXIS_RIGHT_X: snprintf(buffer, buffer_size, "GPAD_AXIS_RX"); return buffer;
				case GLFW_GAMEPAD_AXIS_RIGHT_Y: snprintf(buffer, buffer_size, "GPAD_AXIS_RY"); return buffer;
				case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: snprintf(buffer, buffer_size, "GPAD_AXIS_LT"); return buffer;
				case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: snprintf(buffer, buffer_size, "GPAD_AXIS_RT"); return buffer;
				default: snprintf(buffer, buffer_size, "GPAD_AXIS_%d", input.input); return buffer;
			}
		}
		case GCTK_DEVICE_GAMEPAD_HAT: {
			switch (input.input) {
				case GLFW_HAT_UP: snprintf(buffer, buffer_size, "GPAD_HAT_UP"); return buffer;
				case GLFW_HAT_RIGHT_UP: snprintf(buffer, buffer_size, "GPAD_HAT_RIGHT_UP"); return buffer;
				case GLFW_HAT_RIGHT: snprintf(buffer, buffer_size, "GPAD_HAT_RIGHT"); return buffer;
				case GLFW_HAT_RIGHT_DOWN: snprintf(buffer, buffer_size, "GPAD_HAT_RIGHT_DOWN"); return buffer;
				case GLFW_HAT_DOWN: snprintf(buffer, buffer_size, "GPAD_HAT_DOWN"); return buffer;
				case GLFW_HAT_LEFT_DOWN: snprintf(buffer, buffer_size, "GPAD_HAT_LEFT_DOWN"); return buffer;
				case GLFW_HAT_LEFT: snprintf(buffer, buffer_size, "GPAD_HAT_LEFT"); return buffer;
				case GLFW_HAT_LEFT_UP: snprintf(buffer, buffer_size, "GPAD_HAT_LEFT_UP"); return buffer;
				default: return NULL;
			}
		}
	}
	return NULL;
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

	if (action == NULL || strlen(action) == 0) {
		GctkLogError(GCTK_ERROR_UNDEFINED, "Failed to set input action: Input action id is null or empty!");
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
			GctkStrCpy(GCTK_INPUT_MAP[i].name, action, 128);
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
		GctkStrCpy(GCTK_INPUT_MAP[GCTK_INPUT_MAP_COUNT].name, action, 128);
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

	GctkLogError(GCTK_ERROR_UNDEFINED, "Attempt to get undefined input action \"%s\"", name);
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

bool GctkInputActionPressed(const char* action) { return GctkInputActionState(action) == GCTK_STATE_PRESSED; }
bool GctkInputActionDown(const char* action) { return GctkInputActionState(action) == GCTK_STATE_DOWN; }
bool GctkInputActionReleased(const char* action) { return GctkInputActionState(action) == GCTK_STATE_RELEASED; }
bool GctkInputActionUp(const char* action) { return GctkInputActionState(action) == GCTK_STATE_UP; }