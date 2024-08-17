#include "gctk/rendering/render_queue.h"

static RenderCall GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_MAX_SIZE];
static size_t GCTK_SPRITE_QUEUE_SIZE = 0;

extern Mat4 GctkGetViewportMatrix();

bool GctkRenderEnqueueSprite2D(const Sprite* sprite, Color color, Transform2D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform2DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
		.sprite = sprite,
		.color = color,
		.transform2D = transform,
		.type = GCTK_RENDERCALL_SPRITE_2D
	};
	return true;
}
bool GctkRenderEnqueueSprite2DNoTint(const Sprite* sprite, Transform2D transform) {
	return GctkRenderEnqueueSprite2D(sprite, COLOR_WHITE, transform);
}
bool GctkRenderEnqueueSprite3D(const Sprite* sprite, Color color, Transform3D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform3DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
		.sprite = sprite,
		.color = color,
		.transform3D = transform,
		.type = GCTK_RENDERCALL_SPRITE_3D
	};
	return true;
}
bool GctkRenderEnqueueSprite3DNoTint(const Sprite* sprite, Transform3D transform) {
	return GctkRenderEnqueueSprite3D(sprite, COLOR_WHITE, transform);
}
bool GctkRenderEnqueueModel(const Mesh* mesh, Transform3D transform) {
	if (GCTK_SPRITE_QUEUE_SIZE == GCTK_SPRITE_QUEUE_MAX_SIZE) return false;

	Transform3DUpdateMatrix(&transform);
	GCTK_SPRITE_QUEUE[GCTK_SPRITE_QUEUE_SIZE++] = (RenderCall) {
			.mesh = mesh,
			.transform3D = transform,
			.type = GCTK_RENDERCALL_MESH
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

bool GctkRenderDequeueAndRender() {
	const RenderCall* render = GctkRenderDequeue();
	if (render == NULL) {
		return false;
	}

	switch (render->type) {
		case GCTK_RENDERCALL_SPRITE_2D: {
			GctkDrawSprite(render->sprite, render->color, &render->transform2D);
		} break;
		case GCTK_RENDERCALL_SPRITE_3D: {
			GctkDrawSprite(render->sprite, render->color, &render->transform3D);
		} break;
		case GCTK_RENDERCALL_MESH: {
			GctkDrawMesh(render->mesh, render->transform3D.matrix, GctkGetViewportMatrix());
		} break;
		default: return false;
	}
	return true;
}