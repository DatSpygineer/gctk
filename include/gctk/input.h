#ifndef GCTK_INPUT_H
#define GCTK_INPUT_H

#include "gctk/common.h"
#include "gctk/math.h"

typedef enum {
	GCTK_STATE_UNDEFINED = -1,

	GCTK_STATE_UP       = 0,
	GCTK_STATE_PRESSED    = 1,
	GCTK_STATE_DOWN     = 2,
	GCTK_STATE_RELEASED = 3
} InputState;

typedef enum {
	GCTK_DEVICE_NONE,
	GCTK_DEVICE_KEYBOARD,
	GCTK_DEVICE_MOUSE_BUTTON,
	GCTK_DEVICE_MOUSE_WHEEL,
	GCTK_DEVICE_MOUSE_MOTION,
	GCTK_DEVICE_GAMEPAD_BUTTON,
	GCTK_DEVICE_GAMEPAD_AXIS,
	GCTK_DEVICE_GAMEPAD_HAT
} InputDevice;

typedef enum {
	GCTK_GAMEPAD_ANY = -1,
	GCTK_GAMEPAD_0   = 0,
	GCTK_GAMEPAD_1   = 1,
	GCTK_GAMEPAD_2   = 2,
	GCTK_GAMEPAD_3   = 3,
	GCTK_GAMEPAD_4   = 4,
	GCTK_GAMEPAD_5   = 5,
	GCTK_GAMEPAD_6   = 6,
	GCTK_GAMEPAD_7   = 7,
	GCTK_GAMEPAD_LAST = GCTK_GAMEPAD_7
} GamepadDeviceIndex;

typedef enum {
	GCTK_INPUT_AXIS_X_PLUS,
	GCTK_INPUT_AXIS_X_MINUS,
	GCTK_INPUT_AXIS_Y_PLUS,
	GCTK_INPUT_AXIS_Y_MINUS,
	GCTK_INPUT_AXIS_Z_PLUS,
	GCTK_INPUT_AXIS_Z_MINUS
} InputAxis;

typedef enum {
	GCTK_MOUSE_X_PLUS,
	GCTK_MOUSE_X_MINUS,
	GCTK_MOUSE_Y_PLUS,
	GCTK_MOUSE_Y_MINUS
} MouseAxis;

typedef struct {
	int input;
	InputDevice device;
	GamepadDeviceIndex device_id;
} Input;

#ifndef GCTK_INPUT_MAP_MAX_COUNT
	#define GCTK_INPUT_MAP_MAX_COUNT 128
#endif

#define INPUT_NONE ((Input){ 0 })
#define INPUT_IS_NONE(__input__) ((__input__).input == 0 && (__input__).device == 0 && (__input__).device_id == 0)

GCTK_API Input GctkInputFromString(const char* input, int device_id);
GCTK_API char* GctkInputToString(char* buffer, size_t buffer_size, Input input);

GCTK_API bool GctkInputMapExists();

GCTK_API bool GctkLoadInputMap();
GCTK_API bool GctkWriteInputMap();

GCTK_API void GctkSetupInputCallbacks();
GCTK_API void GctkUpdateInputStates();

GCTK_API Vec2 GctkGetMousePosition();
GCTK_API Vec2 GctkGetMousePositionRelative();
GCTK_API Vec2 GctkGetMouseWheel();

GCTK_API bool GctkSetInputAction(const char* action, size_t input_count, const Input* inputs);
GCTK_API bool GctkSetInputActionVA(const char* action, size_t input_count, ...);
GCTK_API bool GctkInputActionDefined(const char* action);

GCTK_API InputState GctkInputActionState(const char* action);
GCTK_API float      GctkInputGetValue(const char* action);

GCTK_API float GctkInputGetAxis(const char* negative_action, const char* positive_action);
GCTK_API Vec2 GctkInputGetVector(
		const char* negative_x_action, const char* positive_x_action,
		const char* negative_y_action, const char* positive_y_action
);

GCTK_API bool GctkInputActionPressed(const char* action);
GCTK_API bool GctkInputActionDown(const char* action);
GCTK_API bool GctkInputActionReleased(const char* action);
GCTK_API bool GctkInputActionUp(const char* action);

#define INPUT_KEY(__key__) ((Input){ .input = (__key__), .device = GCTK_DEVICE_KEYBOARD, .device_id = 0 })
#define INPUT_MOUSE_BTN(__btn__) ((Input){ .input = (__btn__), .device = GCTK_DEVICE_MOUSE_BUTTON, .device_id = 0 })
#define INPUT_MOUSE_WHEEL(__axis__) ((Input){ .input = (__axis__), .device = GCTK_DEVICE_MOUSE_WHEEL, .device_id = 0 })
#define INPUT_MOUSE_MOTION(__axis__) ((Input){ .input = (__axis__), .device = GCTK_DEVICE_MOUSE_MOTION, .device_id = 0 })
#define INPUT_GPAD_BTN(__btn__, __device__) ((Input){ .input = (__btn__), .device = GCTK_DEVICE_GAMEPAD_BUTTON, .device_id = (__device__) })
#define INPUT_GPAD_AXIS(__axis__, __device__) ((Input){ .input = (__axis__), .device = GCTK_DEVICE_GAMEPAD_AXIS, .device_id = (__device__) })
#define INPUT_GPAD_HAT(__hat__, __device__) ((Input){ .input = (__hat__), .device = GCTK_DEVICE_GAMEPAD_HAT, .device_id = (__device__) })

#endif