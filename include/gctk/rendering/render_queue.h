#ifndef GCTK_RENDER_QUEUE_H
#define GCTK_RENDER_QUEUE_H

#include "gctk/common.h"
#include "gctk/rendering/sprite.h"

#ifndef GCTK_SPRITE_QUEUE_MAX_SIZE
	#define GCTK_SPRITE_QUEUE_MAX_SIZE 128
#endif

typedef struct {
	union {
		struct {
			const Sprite* sprite;
			Color color;
		};
		const Mesh* mesh;
	};
	union {
		Transform2D transform2D;
		Transform3D transform3D;
	};
	bool is_3d;
	bool is_model;
} RenderCall;

GCTK_API bool GctkRenderEnqueueSprite2D(const Sprite* sprite, Color color, Transform2D transform);
GCTK_API bool GctkRenderEnqueueSprite3D(const Sprite* sprite, Color color, Transform3D transform);
GCTK_API bool GctkRenderEnqueueModel(const Mesh* mesh, Transform3D transform);

GCTK_API bool GctkRenderQueueIsEmpty();
GCTK_API bool GctkRenderQueueIsFull();
GCTK_API const RenderCall* GctkRenderDequeue();

#endif