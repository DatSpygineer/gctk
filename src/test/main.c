#include "gctk/gctk.h"

static Texture TEXTURE;

static Transform2D TRANSFORM = { .position = VEC2(200, 200), .scale = VEC2(100, 100) };
static Sprite SPRITE;

static void render(double delta) {
	Transform2DUpdateMatrix(&TRANSFORM);
	GctkSpriteDraw(&SPRITE, COLOR(1.0f, 1.0f, 1.0f, 1.0f), &TRANSFORM);
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "Test", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return GctkLastErrorCode();
	}

	GctkSetRenderCallback(&render);

	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");

	SPRITE = GctkCreateSprite(GctkSprite2DDefaultShader(), &TEXTURE);

	while (GctkUpdate());

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteSprite(&SPRITE);
	GctkDispose();

	return 0;
}