#include "gctk/game.h"

#include "gctk/str.h"
#include "gctk/debug.h"
#include "gctk/filesys.h"
#include "gctk/rendering/viewport.h"
#include "gctk/rendering/sprite.h"
#include "gctk/rendering/render_queue.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static GLFWwindow* GCTK_WINDOW = NULL;

static char GCTK_BASE_DIR[GCTK_PATH_MAX] = { 0 };
static char GCTK_NAME[512] = { 0 };
static char GCTK_AUTHOR[512] = { 0 };

static Version GCTK_VERSION = { 0 };
static void (*GCTK_UPDATE_CALLBACK)(double) = NULL;
static void (*GCTK_RENDER_CALLBACK)(double) = NULL;
static void (*GCTK_CLOSE_CALLBACK)() = NULL;

static double GCTK_LAST_TIME = 0, GCTK_DELTA_TIME = 0;

static Color GCTK_BACKGROUND_COLOR = COLOR(0.25f, 0.5f, 1.0f, 1.0f);

static void GctkWindowResizeCallback(GLFWwindow* window, int width, int height) {
	GctkUpdateViewport(width, height);
}
static void GctkGlfwErrorCallback(int code, const char* message) {
	GctkLogFatal(GCTK_ERROR_GLFW_FAILURE, "GLFW error: %s (%d)", message, code);
}

extern void GctkSprite2DDeleteDefaultShader();

const Version GCTK_ENGINE_VERSION = VERSION(
	1, 0, 0, GCTK_VERSION_ALPHA
);
const char* GCTK_ENGINE_VERSION_STRING = "1.0 ALPHA";

Version GctkGetGameVersion() {
	return GCTK_VERSION;
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
	GCTK_VERSION = game_version;
	GctkStrCpy(GCTK_NAME, name, 512);
	GctkStrCpy(GCTK_AUTHOR, author, 512);
	GctkPathGetBase(GCTK_BASE_DIR, argv[0]);

	glfwSetErrorCallback(&GctkGlfwErrorCallback);

	if (glfwInit() == GLFW_FALSE) {
		const char* error_desc;
		int code = glfwGetError(&error_desc);
		GctkLogFatal(GCTK_ERROR_GLFW_FAILURE, "Failed to initialize GLFW: %s (%c)", error_desc, code);
		return false;
	}

	int w = 1360;
	int h = 768;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GCTK_WINDOW = glfwCreateWindow(w, h, name, NULL, NULL);
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
	char debug_title[1025] = { 0 };
	snprintf(debug_title, 1024, "%s | DEBUG | Gctk v%s | OpenGL v%s", name,
			 GCTK_ENGINE_VERSION_STRING,
			 glGetString(GL_VERSION)
	);
	glfwSetWindowTitle(GCTK_WINDOW, debug_title);
#endif

	glfwSetWindowSizeCallback(GCTK_WINDOW, &GctkWindowResizeCallback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GctkDebugEnableGLDebug();

	GctkSetupViewport2D(w, h, VEC2_ZERO, -100, 100);

	return true;
}

bool GctkUpdate() {
	double time = glfwGetTime();
	glfwPollEvents();
	GCTK_DELTA_TIME = time - GCTK_LAST_TIME;

	if (GCTK_UPDATE_CALLBACK != NULL) GCTK_UPDATE_CALLBACK(GCTK_DELTA_TIME);

	glClearColor(GCTK_BACKGROUND_COLOR.r, GCTK_BACKGROUND_COLOR.g, GCTK_BACKGROUND_COLOR.b, GCTK_BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (GCTK_RENDER_CALLBACK != NULL) GCTK_RENDER_CALLBACK(GCTK_DELTA_TIME);

	while (!GctkRenderQueueIsEmpty()) {
		const RenderCall* call = GctkRenderDequeue();
		if (call->is_model) {
			GctkDrawMesh(call->mesh, *(call->is_3d ?
									(const Mat4*)&call->transform3D :
									(const Mat4*) &call->transform2D),
									GctkGetViewportMatrix()
			);
		} else {
			GctkSpriteDraw(call->sprite, call->color,
						   call->is_3d ? (void*) &call->transform3D : (void*) &call->transform2D);
		}
	}

	glfwSwapBuffers(GCTK_WINDOW);
	GCTK_LAST_TIME = time;

	return !glfwWindowShouldClose(GCTK_WINDOW);
}
void GctkDispose() {
	if (GCTK_CLOSE_CALLBACK != NULL) GCTK_CLOSE_CALLBACK();

	GctkSprite2DDeleteDefaultShader();
	if (GCTK_WINDOW != NULL) glfwDestroyWindow(GCTK_WINDOW);
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
void GctkSetCloseCallback(void (*callback)()) {
	GCTK_CLOSE_CALLBACK = callback;
}

double GctkTime() {
	return glfwGetTime();
}
double GctkDeltaTime() {
	return GCTK_DELTA_TIME;
}