#include "gctk/debug.h"
#include "gctk/str.h"
#include "gctk/filesys.h"

#include "tinyfiledialogs.h"

#include <GL/glew.h>

#include <time.h>

extern void GctkDispose();

static FILE* GCTK_DEBUG_LOG = NULL;

static GctkErrorCode GCTK_ERROR_CODE = GCTK_OK;
static char GCTK_ERROR_MESSAGE[GCTK_MESSAGE_LOG_BUFFER_SIZE] = { 0 };
static char GCTK_LOG_PATH[GCTK_PATH_MAX] = { 0 };

static MessageCallbackFn GCTK_DEBUG_MESSAGE_CALLBACK = NULL;

extern void GctkGetBaseDirectory(char* buffer);

bool GctkSetupDebugLogger() {
	if (GCTK_DEBUG_LOG != NULL) {
		fclose(GCTK_DEBUG_LOG);
	}
	GCTK_DEBUG_LOG = NULL;
	GctkGetBaseDirectory(GCTK_LOG_PATH);

	GctkPathAppend(GCTK_LOG_PATH, "logs");
	if (!GctkPathExists(GCTK_LOG_PATH)) {
		if (!GctkCreateDir(GCTK_LOG_PATH, true)) {
			return false;
		}
	}

	char filename[128] = { 0 };
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	strftime(filename, 128, "log_%Y_%m_%d.log", lt);
	GctkPathAppend(GCTK_LOG_PATH, filename);

#ifndef NDEBUG
	printf("Debug log path ==> \"%s\"\n", GCTK_LOG_PATH);
#endif

	GCTK_DEBUG_LOG = fopen(GCTK_LOG_PATH, "a+");

	if (GCTK_DEBUG_LOG != NULL) {
		char date[32] = { 0 };
		strftime(date, 32, "%Y.%m.%d %H:%M:%S", lt);
		fprintf(GCTK_DEBUG_LOG, "==== Game instance started [%s] ====\n", date);
		return true;
	}
	return false;
}
void GctkCloseDebugLogger() {
	if (GCTK_DEBUG_LOG != NULL) {
		time_t t = time(NULL);
		struct tm* lt = localtime(&t);
		char date[32] = { 0 };
		strftime(date, 32, "%Y.%m.%d %H:%M:%S", lt);
		fprintf(GCTK_DEBUG_LOG, "==== Game instance closed [%s] ====\n", date);
		fflush(GCTK_DEBUG_LOG);
		fclose(GCTK_DEBUG_LOG);
	}
	GCTK_DEBUG_LOG = NULL;
}

GctkErrorCode GctkLastErrorCode() {
	return GCTK_ERROR_CODE;
}

void GctkDispatchDebugMessage(GctkDebugMessageType type, GctkErrorCode error_code, GctkDebugInfo debug_info,
							  const char* format, ...) {
	va_list args;
	va_start(args, format);
	GctkDispatchDebugMessageV(type, error_code, debug_info, format, args);
	va_end(args);
}
void GctkDispatchDebugMessageV(GctkDebugMessageType type, GctkErrorCode error_code, GctkDebugInfo debug_info,
							   const char* format, va_list args) {
	char message[GCTK_MESSAGE_LOG_BUFFER_SIZE] = { 0 };
	vsnprintf(message, GCTK_MESSAGE_LOG_BUFFER_SIZE, format, args);

	char date[64] = { 0 };
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	strftime(date, 64, "%Y.%m.%d %X", lt);

	const char* label;
	switch (type) {
		case GCTK_MESSAGE_INFO: label = "INFO"; break;
		case GCTK_MESSAGE_WARN: label = "WARN"; break;
		case GCTK_MESSAGE_ERROR: label = "ERROR"; break;
		case GCTK_MESSAGE_FATAL: label = "FATAL"; break;
	}

	GCTK_ERROR_CODE = error_code;
	if (type >= GCTK_MESSAGE_ERROR) {
		GctkStrCpy(GCTK_ERROR_MESSAGE, message, GCTK_MESSAGE_LOG_BUFFER_SIZE);
	}

	if (GCTK_DEBUG_LOG != NULL) {
		fprintf(GCTK_DEBUG_LOG, "%s [%s] %s:%zu (%s): %s\n", date, label,
				debug_info.file, debug_info.line, debug_info.func, message);
	}
#if defined(GCTK_DEBUG_LOG_TO_CONSOLE) && GCTK_DEBUG_LOG_TO_CONSOLE
	printf("%s [%s] %s:%zu (%s): %s\n", date, label,
			debug_info.file, debug_info.line, debug_info.func, message);
#endif

	if (GCTK_DEBUG_MESSAGE_CALLBACK != NULL) {
		GCTK_DEBUG_MESSAGE_CALLBACK(type, message, debug_info);
	}

	if (type == GCTK_MESSAGE_FATAL) {
		GctkCrash("Engine error: %s", GCTK_ERROR_MESSAGE);
	}
}

void GctkSetMessageCallback(MessageCallbackFn callback) {
	GCTK_DEBUG_MESSAGE_CALLBACK = callback;
}

bool GctkDispatchDebugAssert(bool expression_result, const char* expression_str, GctkDebugInfo debug_info,
							 GctkErrorCode error_code, bool fatal, const char* message, ...) {
	if (!expression_result) {
		va_list args;
		va_start(args, message);
		GctkDispatchDebugMessageV(fatal ? GCTK_MESSAGE_FATAL : GCTK_MESSAGE_ERROR, error_code, debug_info, message, args);
		va_end(args);
	}
	return expression_result;
}

void GctkDebugEnableGLDebug() {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

void GctkCheckGLStatusImplementation(const char* glFunction, GctkDebugInfo debugInfo) {
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		bool header = false;
		GLuint count = glGetDebugMessageLog(1, 0, NULL, NULL, NULL, NULL, NULL, NULL);
		while (count > 0) {
			GLenum sources[1];
			GLenum types[1];
			GLuint ids[1];
			GLenum severities[1];
			GLsizei lengths[1];
			GLchar messageLog[256];

			glGetDebugMessageLog(1, sizeof(messageLog), sources, types, ids, severities, lengths, messageLog);

			if (*severities != GL_DEBUG_TYPE_ERROR) {
				goto NEXT;
			}

			if (!header) {
				GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_GL_RUNTIME, debugInfo,
										 "GL call \"%s\" has failed, OpenGL runtime error (%d):", glFunction, error
				);
				header = true;
			}
			printf("\t%d: %s\n", ids[0], messageLog);
			NEXT:
			count = glGetDebugMessageLog(1, 0, NULL, NULL, NULL, NULL, NULL, NULL);
		}
	}
}

void GctkCrash(const char* crash_message_format, ...) {
	char date[64] = { 0 };
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	strftime(date, 64, "%Y.%m.%d %X", lt);

	char crash_message[1024] = { 0 };
	va_list args;
	va_start(args, crash_message_format);
	vsnprintf(crash_message, 1024, crash_message_format, args);
	va_end(args);
	printf("%s - Game has crashed! %s\n", date, crash_message);
	if (GCTK_DEBUG_LOG != NULL) {
		fprintf(GCTK_DEBUG_LOG, "%s - Game has crashed! %s\n", date, crash_message);
	}

	tinyfd_messageBox("Fatal error!", crash_message, "ok", "error", 0);

	char crash_path[GCTK_PATH_MAX] = { 0 };
	GctkGetBaseDirectory(crash_path);
	GctkPathAppend(crash_path, "crash");
	if (!GctkPathExists(crash_path)) {
		if (!GctkCreateDir(crash_path, true)) {
			goto EXIT;
		}
	}

	char filename[128] = { 0 };
	t = time(NULL);
	lt = localtime(&t);
	strftime(filename, 128, "crash_%Y_%m_%d.log", lt);
	GctkPathAppend(crash_path, filename);

	int file_index = 1;
	while (GctkPathExists(crash_path)) {
		GctkPathPop(crash_path);
		strftime(date, 64, "%Y_%m_%d", lt);
		snprintf(filename, 128, "%s_%03d.log", date, file_index++);
		GctkPathAppend(crash_path, filename);
	}

	if (GCTK_DEBUG_LOG != NULL) {
		fflush(GCTK_DEBUG_LOG);
		fclose(GCTK_DEBUG_LOG);
		GCTK_DEBUG_LOG = NULL;
	}
	GctkCopyFile(GCTK_LOG_PATH, crash_path, true);
EXIT:
	GctkDispose();
	exit(GCTK_ERROR_CODE == GCTK_OK ? -1 : GCTK_ERROR_CODE);
}
