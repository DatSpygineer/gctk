#ifndef GCTK_GAME_H
#define GCTK_GAME_H

#include "gctk/common.h"
#include "gctk/filesys.h"
#include "gctk/math.h"

typedef enum VersionLifeCycle {
	GCTK_VERSION_RELEASE,
	GCTK_VERSION_ALPHA,
	GCTK_VERSION_BETA,
	GCTK_VERSION_PRE_RELEASE
} VersionLifeCycle;

typedef struct Version {
	uint8_t major, minor, patch, life_cycle;
} Version;

#define VERSION(__major__, __minor__, __pitch__, __life_cycle__) ((Version){ __major__, __minor__, __pitch__, __life_cycle__ })

GCTK_API_CONST Version GCTK_ENGINE_VERSION;

GCTK_API Version GctkGetGameVersion();

GCTK_API int GctkGetVersionString(char* buffer, size_t max_size, const Version* version);

GCTK_API bool GctkInit(int argc, char** argv, const char* name, const char* author, Version game_version);
GCTK_API bool GctkUpdate();
GCTK_API void GctkDispose();

GCTK_API void GctkClose();
GCTK_API void GctkGetBaseDirectory(char* buffer);
GCTK_API void GctkGetUserDirectory(char* buffer);

GCTK_API Color GctkGetBackgroundColor();
GCTK_API void GctkSetBackgroundColor(Color color);

GCTK_API void GctkSetUpdateCallback(void (*callback)(double));
GCTK_API void GctkSetPreRenderCallback(void (*callback)(double));
GCTK_API void GctkSetRenderCallback(void (*callback)(double));
GCTK_API void GctkSetPostRenderCallback(void (*callback)(double));
GCTK_API void GctkSetCloseCallback(void (*callback)());

GCTK_API double GctkTime();
GCTK_API double GctkDeltaTime();

GCTK_API void* GctkGetWindowHandle();

GCTK_API Vec2i GctkGetWindowSize();
GCTK_API Vec2i GctkGetWindowPos();

#endif