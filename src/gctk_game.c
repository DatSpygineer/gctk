#include "gctk/game.h"

#include "gctk/str.h"
#include "gctk/debug.h"
#include "gctk/input.h"
#include "gctk/filesys.h"
#include "gctk/collections.h"
#include "gctk/rendering/viewport.h"
#include "gctk/rendering/render_queue.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cjson/cJSON.h>

typedef struct {
	Vec2i resolution;
	bool fullscreen;
	int monitor_idx;
} GameConfig;

static GLFWwindow* GCTK_WINDOW = NULL;

static char GCTK_BASE_DIR[GCTK_PATH_MAX] = { 0 };
static char GCTK_NAME[512] = { 0 };
static char GCTK_AUTHOR[512] = { 0 };

static Version GCTK_GAME_VERSION = { 0 };
static void (*GCTK_UPDATE_CALLBACK)(double) = NULL;
static void (*GCTK_RENDER_CALLBACK)(double) = NULL;
static void (*GCTK_PRE_RENDER_CALLBACK)(double) = NULL;
static void (*GCTK_POST_RENDER_CALLBACK)(double) = NULL;
static void (*GCTK_CLOSE_CALLBACK)() = NULL;

static double GCTK_LAST_TIME = 0, GCTK_DELTA_TIME = 0;

static Color GCTK_BACKGROUND_COLOR = COLOR(0.25f, 0.5f, 1.0f, 1.0f);

static GameConfig GCTK_CONFIG;

static void GctkWindowResizeCallback(GLFWwindow* window, int width, int height) {
	GctkUpdateViewport(width, height);
}
static void GctkGlfwErrorCallback(int code, const char* message) {
	GctkLogFatal(GCTK_ERROR_GLFW_FAILURE, "GLFW error: %s (%d)", message, code);
}

extern void GctkSprite2DDeleteDefaultShader();
extern void GctkAnimation2DDeleteDefaultShader();

const Version GCTK_ENGINE_VERSION = VERSION(
	1, 0, 0, GCTK_VERSION_ALPHA
);
const char* GCTK_ENGINE_VERSION_STRING = "1.0 ALPHA";

Version GctkGetGameVersion() {
	return GCTK_GAME_VERSION;
}

int GctkGetVersionString(char* buffer, size_t max_size, const Version* version) {
	int result = 0;
	if (version->patch > 0) {
		result = snprintf(buffer, max_size, "%d.%d.%d", version->major, version->minor, version->patch);
	} else {
		result = snprintf(buffer, max_size, "%d.%d", version->major, version->minor);
	}
	if (result < 0) {
		return result;
	}

	char lc[14] = { 0 };
	switch (version->life_cycle) {
		case GCTK_VERSION_ALPHA: GctkStrCpy(lc, " ALPHA", 14); break;
		case GCTK_VERSION_BETA: GctkStrCpy(lc, " BETA", 14); break;
		case GCTK_VERSION_PRE_RELEASE: GctkStrCpy(lc, " PRE-RELEASE", 14); break;
	}
	if (GctkStrAppend(buffer, lc, max_size) == NULL) {
		return -1;
	}
	return result + (int)strlen(lc);
}

bool GctkInit(int argc, char** argv, const char* name, const char* author, Version game_version) {
	GCTK_GAME_VERSION = game_version;
	GctkStrCpy(GCTK_NAME, name, 512);
	GctkStrCpy(GCTK_AUTHOR, author, 512);
	GctkPathGetBase(GCTK_BASE_DIR, argv[0]);

	char path_temp[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path_temp);
	if (!GctkPathIsDirectory(path_temp) && !GctkCreateDir(path_temp, true)) {
		GctkLogFatal(GCTK_ERROR_IO_FAILURE, "Failed to create user directory \"%s\"", path_temp);
		return false;
	}

	memset(path_temp, 0, GCTK_PATH_MAX);
	GctkGetUserDirectory(path_temp);
	GctkPathAppend(path_temp, "settings.json");
	if (GctkPathExists(path_temp)) {
		FILE* f = GctkOpenFile(path_temp, GCTK_FILEMODE_READ, GCTK_FILE_OPEN);
		const size_t size = GctkFileSize(f);
		char* p = (char*)malloc(size + 1);
		memset(p, 0, size + 1);
		GctkFileRead(f, p, size, sizeof(char));
		cJSON* json = cJSON_Parse(p);
		free(p);
		fclose(f);
		if (json == NULL) {
			GctkLogFatal(GCTK_ERROR_PARSE_FAILED, "Failed to read config file \"%s\"", path_temp);
			return false;
		}

		const cJSON* resolution = cJSON_GetObjectItem(json, "resolution");
		if (resolution != NULL && cJSON_IsArray(resolution)) {
			const cJSON* x = cJSON_GetArrayItem(resolution, 0);
			const cJSON* y = cJSON_GetArrayItem(resolution, 1);
			if (!cJSON_IsNumber(x) || !cJSON_IsNumber(y)) {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get window resolution! Expected an array of 2 numbers!");
				GCTK_CONFIG.resolution = VEC2I(1360, 768);
			} else {
				GCTK_CONFIG.resolution = VEC2I((int)x->valuedouble, (int)y->valuedouble);
			}
		} else {
			GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get window resolution! Expected an array of 2 numbers!");
			GCTK_CONFIG.resolution = VEC2I(1360, 768);
		}

		const cJSON* fullscreen = cJSON_GetObjectItem(json, "fullscreen");
		if (fullscreen) {
			if (cJSON_IsBool(fullscreen)) {
				GCTK_CONFIG.fullscreen = fullscreen->valueint > 0;
			} else if (cJSON_IsNumber(fullscreen)) {
				GCTK_CONFIG.fullscreen = fullscreen->valuedouble > 0;
			} else {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get fullscreen! Expected a boolean value!");
				GCTK_CONFIG.fullscreen = false;
			}
		} else {
			GctkLogWarn("Optional settings entry \"fullscreen\" is not present. Defaulting to false.");
			GCTK_CONFIG.fullscreen = false;
		}

		const cJSON* monitor_idx = cJSON_GetObjectItem(json, "monitor_idx");
		if (monitor_idx) {
			if (cJSON_IsNumber(monitor_idx)) {
				GCTK_CONFIG.monitor_idx = (int)monitor_idx->valuedouble;
			} else {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get monitor_idx! Expected a numeric value!");
				GCTK_CONFIG.monitor_idx = -1;
			}
		} else {
			GctkLogWarn("Optional settings entry \"monitor_idx\" is not present. Defaulting to -1.");
			GCTK_CONFIG.monitor_idx = -1;
		}

		cJSON_Delete(json);
	} else {
		GCTK_CONFIG = (GameConfig) {
			.resolution = VEC2I(1360, 768),
			.fullscreen = false,
			.monitor_idx = -1
		};
	}

	glfwSetErrorCallback(&GctkGlfwErrorCallback);
	GctkSetupDebugLogger();

	if (glfwInit() == GLFW_FALSE) {
		const char* error_desc;
		int code = glfwGetError(&error_desc);
		GctkLogFatal(GCTK_ERROR_GLFW_FAILURE, "Failed to initialize GLFW: %s (%c)", error_desc, code);
		return false;
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GCTK_WINDOW = glfwCreateWindow(GCTK_CONFIG.resolution.x, GCTK_CONFIG.resolution.y, name, NULL, NULL);
	if (GCTK_WINDOW == NULL) {
		const char* error_desc;
		int code = glfwGetError(&error_desc);
		glfwTerminate();

		GctkLogFatal(GCTK_ERROR_GLFW_FAILURE, "Failed to create window: %s (%c)", error_desc, code);
		return false;
	}
	glfwMakeContextCurrent(GCTK_WINDOW);

	glewExperimental = GL_TRUE;
	GLenum glew_err;
	if ((glew_err = glewInit()) != GLEW_NO_ERROR) {
		const char* error_desc = (const char*)glewGetErrorString(glew_err);
		glfwDestroyWindow(GCTK_WINDOW);
		glfwTerminate();

		GctkLogFatal(GCTK_ERROR_GL_INIT, "Failed to initialize OpenGL: %s (%c)", error_desc, glew_err);
		return false;
	}

#ifndef NDEBUG
	GctkSetTitle(name);
#endif

	glfwSetWindowSizeCallback(GCTK_WINDOW, &GctkWindowResizeCallback);

	GctkSetBlendmode(GCTK_BLEND_ALPHA_MIX);

	GctkSetupInputCallbacks();
	GctkLoadInputMap();

	GctkDebugEnableGLDebug();

	GctkSetupViewport2D(GCTK_CONFIG.resolution.x, GCTK_CONFIG.resolution.y, VEC2_ZERO, -100, 100);

	return true;
}

bool GctkUpdate() {
	double time = glfwGetTime();
	glfwPollEvents();
	GCTK_DELTA_TIME = time - GCTK_LAST_TIME;

	GctkUpdateInputStates();

	if (GCTK_UPDATE_CALLBACK != NULL) GCTK_UPDATE_CALLBACK(GCTK_DELTA_TIME);

	if (GCTK_PRE_RENDER_CALLBACK != NULL) GCTK_PRE_RENDER_CALLBACK(GCTK_DELTA_TIME);

	glClearColor(GCTK_BACKGROUND_COLOR.r, GCTK_BACKGROUND_COLOR.g, GCTK_BACKGROUND_COLOR.b, GCTK_BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (GCTK_RENDER_CALLBACK != NULL) GCTK_RENDER_CALLBACK(GCTK_DELTA_TIME);

	while (!GctkRenderQueueIsEmpty()) {
		GctkRenderDequeueAndRender();
	}

	if (GCTK_POST_RENDER_CALLBACK != NULL) GCTK_POST_RENDER_CALLBACK(GCTK_DELTA_TIME);

	glfwSwapBuffers(GCTK_WINDOW);
	GCTK_LAST_TIME = time;

	return !glfwWindowShouldClose(GCTK_WINDOW);
}

bool GctkLoadSettings() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "settings.json");
	if (GctkPathExists(path)) {
		FILE* f = GctkOpenFile(path, GCTK_FILEMODE_READ, GCTK_FILE_OPEN);
		const size_t size = GctkFileSize(f);
		char* p = (char*)malloc(size + 1);
		memset(p, 0, size + 1);
		GctkFileRead(f, p, size, sizeof(char));
		cJSON* json = cJSON_Parse(p);
		free(p);
		fclose(f);
		if (json == NULL) {
			GctkLogFatal(GCTK_ERROR_PARSE_FAILED, "Failed to read config file \"%s\"", path);
			return false;
		}

		const cJSON* resolution = cJSON_GetObjectItem(json, "resolution");
		if (resolution != NULL && cJSON_IsArray(resolution)) {
			const cJSON* x = cJSON_GetArrayItem(resolution, 0);
			const cJSON* y = cJSON_GetArrayItem(resolution, 1);
			if (!cJSON_IsNumber(x) || !cJSON_IsNumber(y)) {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get window resolution! Expected an array of 2 numbers!");
				GCTK_CONFIG.resolution = VEC2I(1360, 768);
			} else {
				GCTK_CONFIG.resolution = VEC2I((int)x->valuedouble, (int)y->valuedouble);
			}
		} else {
			GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get window resolution! Expected an array of 2 numbers!");
			GCTK_CONFIG.resolution = VEC2I(1360, 768);
		}

		const cJSON* fullscreen = cJSON_GetObjectItem(json, "fullscreen");
		if (fullscreen) {
			if (cJSON_IsBool(fullscreen)) {
				GCTK_CONFIG.fullscreen = fullscreen->valueint > 0;
			} else if (cJSON_IsNumber(fullscreen)) {
				GCTK_CONFIG.fullscreen = fullscreen->valuedouble > 0;
			} else {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get fullscreen! Expected a boolean value!");
				GCTK_CONFIG.fullscreen = false;
			}
		} else {
			GctkLogWarn("Optional settings entry \"fullscreen\" is not present. Defaulting to false.");
			GCTK_CONFIG.fullscreen = false;
		}

		const cJSON* monitor_idx = cJSON_GetObjectItem(json, "monitor_idx");
		if (monitor_idx) {
			if (cJSON_IsNumber(fullscreen)) {
				GCTK_CONFIG.monitor_idx = (int)monitor_idx->valuedouble;
			} else {
				GctkLogError(GCTK_ERROR_PARSE_FAILED, "Failed to get monitor_idx! Expected a numeric value!");
				GCTK_CONFIG.monitor_idx = -1;
			}
		} else {
			GctkLogWarn("Optional settings entry \"monitor_idx\" is not present. Defaulting to -1.");
			GCTK_CONFIG.monitor_idx = -1;
		}

		GctkSetWindowSize(VEC2I(GCTK_CONFIG.resolution.x, GCTK_CONFIG.resolution.y));
		GctkSetWindowFullscreen(GCTK_CONFIG.fullscreen, GCTK_CONFIG.monitor_idx);

		cJSON_Delete(json);
		return true;
	}

	return false;
}
bool GctkWriteSettings() {
	char path[GCTK_PATH_MAX] = { 0 };
	GctkGetUserDirectory(path);
	GctkPathAppend(path, "settings.json");

	Vec2i w_size = GctkGetWindowSize();

	FILE* f = GctkOpenFile(path, GCTK_FILEMODE_WRITE, GCTK_FILE_OPEN_OR_CREATE);
	if (f == NULL) {
		return false;
	}

	fprintf(f, "{\n"
			   "\t\"resolution\": [ %d, %d ],\n"
			   "\t\"fullscreen\": %s,\n"
			   "\t\"monitor_idx\": %d\n"
			   "}\n",
		w_size.x, w_size.y,
		GCTK_CONFIG.fullscreen ? "true" : "false",
		GCTK_CONFIG.monitor_idx
	);
	fclose(f);
	return true;
}

void GctkDispose() {
	GctkWriteSettings();

#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
	GctkReportMemoryLeakCount();
#endif

	if (GCTK_CLOSE_CALLBACK != NULL) GCTK_CLOSE_CALLBACK();

	GctkSprite2DDeleteDefaultShader();
	GctkAnimation2DDeleteDefaultShader();
	if (GCTK_WINDOW != NULL) glfwDestroyWindow(GCTK_WINDOW);

	GctkCloseDebugLogger();
	glfwTerminate();
}

void GctkClose() {
	glfwSetWindowShouldClose(GCTK_WINDOW, GLFW_TRUE);
}

void GctkGetBaseDirectory(char* buffer) {
	if (strlen(GCTK_BASE_DIR) > 0) {
		GctkStrCpy(buffer, GCTK_BASE_DIR, GCTK_PATH_MAX);
	} else {
		GctkPathGetCurrentDir(buffer);
	}
}
void GctkGetUserDirectory(char* buffer) {
	GctkPathGetUserBaseDirectory(buffer);
	GctkPathAppend(buffer, GCTK_AUTHOR);
	GctkPathAppend(buffer, GCTK_NAME);
}

Color GctkGetBackgroundColor() {
	return GCTK_BACKGROUND_COLOR;
}
void GctkSetBackgroundColor(Color color) {
	GCTK_BACKGROUND_COLOR = color;
}

void GctkSetUpdateCallback(void (*callback)(double)) {
	GCTK_UPDATE_CALLBACK = callback;
}
void GctkSetRenderCallback(void (*callback)(double)) {
	GCTK_RENDER_CALLBACK = callback;
}
void GctkSetPreRenderCallback(void (*callback)(double)) {
	GCTK_PRE_RENDER_CALLBACK = callback;
}
void GctkSetPostRenderCallback(void (*callback)(double)) {
	GCTK_POST_RENDER_CALLBACK = callback;
}
void GctkSetCloseCallback(void (*callback)()) {
	GCTK_CLOSE_CALLBACK = callback;
}

double GctkTime() {
	return glfwGetTime();
}
double GctkDeltaTime() {
	return GCTK_DELTA_TIME;
}

void* GctkGetWindowHandle() {
	return (void*)GCTK_WINDOW;
}

void GctkSetWindowPos(Vec2i pos) {
	glfwSetWindowPos(GCTK_WINDOW, pos.x, pos.y);
}
void GctkSetWindowSize(Vec2i size) {
	glfwSetWindowSize(GCTK_WINDOW, size.x, size.y);
}
void GctkSetWindowFullscreen(bool fullscreen, int monitor_idx) {
	if (fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (monitor_idx >= 0) {
			int count;
			GLFWmonitor** monitors = glfwGetMonitors(&count);
			if (monitor_idx < count) {
				monitor = monitors[monitor_idx];
			} else {
				monitor_idx = -1;
			}
		}

		int x, y, w, h;
		glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
		glfwSetWindowMonitor(GCTK_WINDOW, monitor, x, y, w, h, GLFW_DONT_CARE);
	} else {
		Vec2i size = GctkGetWindowSize();
		glfwSetWindowMonitor(GCTK_WINDOW, NULL, 0, 0, size.x, size.y, GLFW_DONT_CARE);
	}
	GCTK_CONFIG.fullscreen = fullscreen;
	GCTK_CONFIG.monitor_idx = monitor_idx;
}

Vec2i GctkGetWindowSize() {
	int w, h;
	glfwGetFramebufferSize(GCTK_WINDOW, &w, &h);
	return (Vec2i){ w, h };
}
Vec2i GctkGetWindowPos() {
	int x, y;
	glfwGetWindowPos(GCTK_WINDOW, &x, &y);
	return (Vec2i){ x, y };
}
int GctkGetWindowMonitorIdx() {
	return GCTK_CONFIG.monitor_idx;
}
bool GctkIsWindowFullscreen() {
	return GCTK_CONFIG.fullscreen;
}

void GctkSetTitle(const char* title) {
#ifndef NDEBUG
	char debug_title[1025] = { 0 };
	char game_version[256] = { 0 };
	GctkGetVersionString(game_version, 256, &GCTK_GAME_VERSION);

	snprintf(debug_title, 1024, "[DEBUG] %s | Game v%s | Gctk v%s | OpenGL v%s", title,
			 game_version,
			 GCTK_ENGINE_VERSION_STRING,
			 glGetString(GL_VERSION)
	);
	glfwSetWindowTitle(GCTK_WINDOW, debug_title);
#else
	glfwSetWindowTitle(GCTK_WINDOW, title);
#endif
}