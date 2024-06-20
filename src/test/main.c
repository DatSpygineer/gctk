#include "gctk/gctk.h"

#include <glfw/glfw3.h>

static Texture TEXTURE;

static Transform2D TRANSFORM = { .position = VEC2_ZERO, .scale = VEC2(100, 100) };
static Sprite SPRITE;

static void update(double delta) {
	Vec2 motion = GctkInputGetVector("MoveLeft", "MoveRight", "MoveDown", "MoveUp");
	if (Vec2LenSqr(motion) != 0.0f) {
		TRANSFORM.position = Vec2Add(TRANSFORM.position, motion);
	}

	if (GctkInputActionPressed("Test")) {
		GctkLog("Test!");
	}

	GctkRenderEnqueueSprite2D(&SPRITE, COLOR(1.0, 1.0, 1.0, 1.0), TRANSFORM);
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "Test", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return GctkLastErrorCode();
	}

	GctkSetUpdateCallback(&update);

	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_FLAG_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");

	GctkAssertFatal(GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE),
					GCTK_ERROR_GL_RUNTIME, "Failed to create sprite!");

	Vec2i size = GctkGetWindowSize();
	TRANSFORM.position = VEC2(((float)size.x) * 0.5f - TRANSFORM.scale.x * 0.5f,
							  ((float)size.y) * 0.5f - TRANSFORM.scale.y * 0.5f);

	while (GctkUpdate());

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteSprite(&SPRITE);
	GctkDispose();

	return 0;
}