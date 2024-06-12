#include "gctk/rendering/render_queue.h"

static RenderCall GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_MAX_SIZE];
static size_t GCTK_SPRITE_QUEUE_SIZE = 0;

bool GctkRenderEnqueueSprite2D(const Sprite* sprite, Color color, Transform2D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform2DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
		.sprite = sprite,
		.color = color,
		.transform2D = transform,
		.is_3d = false,
		.is_model = false
	};
	return true;
}
bool GctkRenderEnqueueSprite3D(const Sprite* sprite, Color color, Transform3D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform3DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
		.sprite = sprite,
		.color = color,
		.transform3D = transform,
		.is_3d = true,
		.is_model = false
	};
	return true;
}
bool GctkRenderEnqueueModel(const Mesh* mesh, Transform3D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform3DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
			.mesh = mesh,
			.transform3D = transform,
			.is_3d = true,
			.is_model = true
	};
	return true;
}

bool GctkRenderQueueIsEmpty() {
	return GCTK_SPRITE_QUEUE_SIZE == 0;
}
bool GctkRenderQueueIsFull() {
	return GCTK_SPRITE_QUEUE_SIZE >= GCTK_SPRITE_QUEUE_MAX_SIZE;
}

const RenderCall* GctkRenderDequeue() {
	if (GCTK_SPRITE_QUEUE_SIZE > 0) {
		return &GCTK_SPRITE_QUEUE[--GCTK_SPRITE_QUEUE_SIZE];
	}

	return NULL;
}