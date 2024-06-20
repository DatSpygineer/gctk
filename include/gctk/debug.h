#ifndef GCTK_DEBUG_H
#define GCTK_DEBUG_H

#include "gctk/common.h"

#ifdef NDEBUG
	#ifndef GCTK_ALLOW_LOG_WARNING
		#define GCTK_ALLOW_LOG_WARNING 1
	#endif
	#ifndef GCTK_ALLOW_LOG_INFO
		#define GCTK_ALLOW_LOG_INFO 1
	#endif
	#ifndef GCTK_DEBUG_LOG_TO_CONSOLE
		#define GCTK_DEBUG_LOG_TO_CONSOLE 0
	#endif
#else
	#ifdef GCTK_ALLOW_LOG_WARNING
		#undef GCTK_ALLOW_LOG_WARNING
	#endif
	#ifdef GCTK_ALLOW_LOG_INFO
		#undef GCTK_ALLOW_LOG_INFO
	#endif
	#ifdef GCTK_DEBUG_LOG_TO_CONSOLE
		#undef GCTK_DEBUG_LOG_TO_CONSOLE
	#endif

	#define GCTK_ALLOW_LOG_WARNING 1
	#define GCTK_ALLOW_LOG_INFO 1
	#define GCTK_DEBUG_LOG_TO_CONSOLE 1
#endif

#ifndef GCTK_MESSAGE_LOG_BUFFER_SIZE
	#define GCTK_MESSAGE_LOG_BUFFER_SIZE 2048
#endif

typedef enum GctkDebugMessageType {
	GCTK_MESSAGE_INFO,
	GCTK_MESSAGE_WARN,
	GCTK_MESSAGE_ERROR,
	GCTK_MESSAGE_FATAL
} GctkDebugMessageType;

typedef enum GctkErrorCode {
	GCTK_OK,
	GCTK_ERROR_OUT_OF_RANGE,
	GCTK_ERROR_OUT_OF_MEMORY,
	GCTK_ERROR_NULL_PTR,
	GCTK_ERROR_REDEFINITION,
	GCTK_ERROR_UNDEFINED,
	GCTK_ERROR_PARSE_FAILED,
	GCTK_ERROR_GLFW_FAILURE,
	GCTK_ERROR_GL_INIT,
	GCTK_ERROR_GL_RUNTIME,
	GCTK_ERROR_LOAD_TEXTURE_FAILURE,
	GCTK_ERROR_COMPILE_SHADER,
	GCTK_ERROR_LOAD_SHADER,
	GCTK_ERROR_LINK_SHADER,
} GctkErrorCode;

typedef struct GctkDebugInfo {
	const char* file;
	const char* func;
	size_t line;
} GctkDebugInfo;

#define GCTK_GET_DEBUG_INFO ((GctkDebugInfo){ __FILE__, __func__, __LINE__ })

GCTK_API bool GctkSetupDebugLogger();
GCTK_API void GctkCloseDebugLogger();

GCTK_API GctkErrorCode GctkLastErrorCode();

GCTK_API void GctkDispatchDebugMessage(GctkDebugMessageType type, GctkErrorCode error_code, GctkDebugInfo debug_info,
									   const char* format, ...);
GCTK_API void GctkDispatchDebugMessageV(GctkDebugMessageType type, GctkErrorCode error_code, GctkDebugInfo debug_info,
										const char* format, va_list args);


#ifdef GCTK_ALLOW_LOG_WARNING
	#define GctkLog(__message__, ...)     GctkDispatchDebugMessage(GCTK_MESSAGE_INFO, GCTK_OK, GCTK_GET_DEBUG_INFO, __message__, ##__VA_ARGS__)
	#define GctkLogWarn(__message__, ...) GctkDispatchDebugMessage(GCTK_MESSAGE_WARN, GCTK_OK, GCTK_GET_DEBUG_INFO, __message__, ##__VA_ARGS__)
#else
	#define GctkLog(__message__, ...)     /* Disabled */
	#define GctkLogWarn(__message__, ...) /* Disabled */
#endif

#define GctkLogError(__code__, __message__, ...) GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, __code__, \
	GCTK_GET_DEBUG_INFO, __message__, ##__VA_ARGS__)
#define GctkLogFatal(__code__, __message__, ...) GctkDispatchDebugMessage(GCTK_MESSAGE_FATAL, __code__, \
	GCTK_GET_DEBUG_INFO, __message__, ##__VA_ARGS__)

GCTK_API bool GctkDispatchDebugAssert(bool expression_result, const char* expression_str, GctkDebugInfo debug_info,
									  GctkErrorCode error_code, bool fatal, const char* message, ...);

#define GctkAssert(__expression__, __code__, __message__, ...) GctkDispatchDebugAssert(__expression__, #__expression__, \
	GCTK_GET_DEBUG_INFO, __code__, false, __message__, ##__VA_ARGS__)
#define GctkAssertFatal(__expression__, __code__, __message__, ...) GctkDispatchDebugAssert(__expression__, #__expression__, \
	GCTK_GET_DEBUG_INFO, __code__, true, __message__, ##__VA_ARGS__)

GCTK_API void GctkDebugEnableGLDebug();
GCTK_API void GctkCheckGLStatusImplementation(const char* glFunction, GctkDebugInfo debugInfo);

#define GctkGLCall(__glcall__) __glcall__; GctkCheckGLStatusImplementation(#__glcall__, GCTK_GET_DEBUG_INFO)

GCTK_API void GctkCrash(const char* crash_message_format, ...);

#endif