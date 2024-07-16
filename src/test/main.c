#include "gctk/gctk.h"

#include <GLFW/glfw3.h>

static Texture TEXTURE;
static Texture TEXTURE_2;
static Texture TEXTURE_3;

static Transform2D TRANSFORM = { .position = VEC2_ZERO, .scale = VEC2(100, 100) };
static Transform2D TRANSFORM_2 = { .position = VEC2_ZERO, .scale = VEC2(100, 100) };
static Transform2D TRANSFORM_3 = { .position = VEC2(200, 0), .scale = VEC2(100, 100) };
static Sprite SPRITE;
static Sprite SPRITE_2;
static Sprite SPRITE_3;

static void update(double delta) {
	Vec2 motion = GctkInputGetVector("MoveLeft", "MoveRight", "MoveDown", "MoveUp");
	if (Vec2LenSqr(motion) != 0.0f) {
		TRANSFORM.position = Vec2Add(TRANSFORM.position, motion);
	}

	if (GctkInputActionPressed("Test")) {
		GctkLog("Test!");
	}

	GctkRenderEnqueueSprite2D(&SPRITE, COLOR_WHITE, TRANSFORM);
	GctkRenderEnqueueSprite2D(&SPRITE_2, COLOR_WHITE, TRANSFORM_2);
	GctkRenderEnqueueSprite2D(&SPRITE_3, COLOR_WHITE, TRANSFORM_3);
}

int main(int argc, char** argv) {
	if (!GctkInitGame(argc, argv)) {
		return GctkLastErrorCode();
	}

	GctkSetUpdateCallback(&update);

	GctkAssertFatal(GctkLoadTextureFromFile(&TEXTURE, "../test.gtex"), GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");
	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE_2, "../test.png", GCTK_IMAGE_FLAG_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");
	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE_3, "../test_2.png", GCTK_IMAGE_FLAG_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");

	GctkAssertFatal(GctkCreateSprite(&SPRITE, GctkSprite2DDefaultShader(), &TEXTURE),
					GCTK_ERROR_GL_RUNTIME, "Failed to create sprite!");
	GctkAssertFatal(GctkCreateSprite(&SPRITE_2, GctkSprite2DDefaultShader(), &TEXTURE_2),
					GCTK_ERROR_GL_RUNTIME, "Failed to create sprite!");
	GctkAssertFatal(GctkCreateSprite(&SPRITE_3, GctkSprite2DDefaultShader(), &TEXTURE_3),
					GCTK_ERROR_GL_RUNTIME, "Failed to create sprite!");

	Vec2i size = GctkGetWindowSize();
	TRANSFORM.position = VEC2(((float)size.x) * 0.5f - TRANSFORM.scale.x * 0.5f,
							  ((float)size.y) * 0.5f - TRANSFORM.scale.y * 0.5f);
	TRANSFORM_2.position = VEC2(0.0f, 0.0f);

	while (GctkUpdate()) { }

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteTexture(&TEXTURE_2);
	GctkDeleteTexture(&TEXTURE_3);
	GctkDeleteSprite(&SPRITE);
	GctkDeleteSprite(&SPRITE_2);
	GctkDeleteSprite(&SPRITE_3);
	GctkDispose();

	return 0;
}