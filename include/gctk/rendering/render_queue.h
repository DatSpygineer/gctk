#ifndef GCTK_RENDER_QUEUE_H
#define GCTK_RENDER_QUEUE_H

#include "gctk/common.h"
#include "gctk/rendering/sprite.h"

#ifndef GCTK_SPRITE_QUEUE_MAX_SIZE
	#define GCTK_SPRITE_QUEUE_MAX_SIZE 128
#endif

typedef enum {
	GCTK_RENDERCALL_SPRITE_2D,
	GCTK_RENDERCALL_ANIMATED_SPRITE_2D,
	GCTK_RENDERCALL_SPRITE_3D,
	GCTK_RENDERCALL_ANIMATED_SPRITE_3D,
	GCTK_RENDERCALL_MESH
} RenderCallType;

typedef struct {
	union {
		struct {
			union {
				const Sprite* sprite;
				AnimatedSprite* animated_sprite;
			};
			Color color;
		};
		const Mesh* mesh;
	};
	union {
		Transform2D transform2D;
		Transform3D transform3D;
	};
	RenderCallType type;
} RenderCall;

GCTK_API bool GctkRenderEnqueueSprite2D(const Sprite* sprite, Color color, Transform2D transform);
GCTK_API bool GctkRenderEnqueueAnimatedSprite2D(const AnimatedSprite* sprite, Color color, Transform2D transform);
GCTK_API bool GctkRenderEnqueueSprite3D(const Sprite* sprite, Color color, Transform3D transform);
GCTK_API bool GctkRenderEnqueueAnimatedSprite3D(const AnimatedSprite* sprite, Color color, Transform3D transform);
GCTK_API bool GctkRenderEnqueueModel(const Mesh* mesh, Transform3D transform);

GCTK_API bool GctkRenderQueueIsEmpty();
GCTK_API bool GctkRenderQueueIsFull();
GCTK_API const RenderCall* GctkRenderDequeue();
GCTK_API bool GctkRenderDequeueAndRender();

#endif