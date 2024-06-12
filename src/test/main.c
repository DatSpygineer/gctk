#include "gctk/gctk.h"

static Texture TEXTURE;

static Transform2D TRANSFORM = { .position = VEC2(200, 200), .scale = VEC2(100, 100) };
static Transform2D TRANSFORM2 = { .position = VEC2(250, 250), .scale = VEC2(100, 100) };
static Sprite SPRITE;

static void update(double delta) {
	GctkRenderEnqueueSprite2D(&SPRITE, COLOR(1.0, 1.0, 1.0, 1.0), TRANSFORM);
	GctkRenderEnqueueSprite2D(&SPRITE, COLOR(1.0, 0.0, 0.0, 1.0), TRANSFORM2);
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "Test", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return GctkLastErrorCode();
	}

	GctkSetUpdateCallback(&update);

	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");

	GctkAssertFatal(GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE),
					GCTK_ERROR_GL_RUNTIME, "Failed to create sprite!");

	while (GctkUpdate());

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteSprite(&SPRITE);
	GctkDispose();

	return 0;
}