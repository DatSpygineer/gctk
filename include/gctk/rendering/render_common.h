#ifndef GCTK_RENDER_COMMON_H
#define GCTK_RENDER_COMMON_H

#include "gctk/common.h"

typedef enum GctkBlendMode {
	GCTK_BLEND_NONE,
	GCTK_BLEND_ADD,
	GCTK_BLEND_MULTIPLY,
	GCTK_BLEND_ALPHA_MIX
} GctkBlendMode;

GCTK_API void GctkSetBlendmode(GctkBlendMode blend_mode);

#endif
