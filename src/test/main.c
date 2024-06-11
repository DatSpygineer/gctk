#include "gctk/gctk.h"

#define STB_IMAGE_IMPLEMENTATION
#include "gctk/stb/stb_image.h"

const char* VERT_SHADER =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aUV;\n"
		"out vec2 UV;\n"
		"\n"
		"void main() {\n"
		"\tgl_Position = vec4(aPos, 1.0);\n"
		"\tUV = aUV;\n"
		"}\n";
const char* FRAG_SHADER =
		"#version 330 core\n"
		"\n"
		"in vec2 UV;\n"
		"out vec4 COLOR;\n"
		"uniform sampler2D TEXTURE;\n"
		"\n"
		"void main() {\n"
		"\tCOLOR = texture(TEXTURE, UV);\n"
		"}\n";

static Shader SHADER;
static Texture TEXTURE;

static float VERTS[] = {
	-0.5f,  0.5f, 0.0f,	0.0f, 0.0f,
	 0.5f,  0.5f, 0.0f,	1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,	1.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,	1.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,	0.0f, 1.0f,
	-0.5f,  0.5f, 0.0f,	0.0f, 0.0f
};

static Mesh MESH;
static Transform2D TRANSFORM = { .position = VEC2(200, 200), .scale = VEC2(100, 100) };
static Sprite SPRITE;

static void render(double delta) {
	Transform2DUpdateMatrix(&TRANSFORM);
//	GctkUpdateViewportUpdateCurrent();
//
//	Mat4 mat = MAT4_IDENTITY;
//
//	GctkBindTexture(&TEXTURE);
//	GctkApplyShader(&SHADER);
//	GctkSetShaderUniformMat4(&SHADER, "TRANSFORM_MATRIX", TRANSFORM.matrix);
//	GctkSetShaderUniformMat4(&SHADER, "VIEW_MATRIX", GctkGetViewportMatrix());
//	GctkSetShaderUniformColor(&SHADER, "COLOR_TINT", COLOR(1.0f, 1.0f, 1.0f, 1.0f));
//	GctkSetShaderUniformTexture(&SHADER, "TEXTURE", &TEXTURE);
//
//	GctkDrawMesh(&MESH);
	GctkSpriteDraw(&SPRITE, COLOR(1.0f, 1.0f, 1.0f, 1.0f), &TRANSFORM);
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "Test", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return GctkLastErrorCode();
	}

	GctkSetRenderCallback(&render);

	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");
	GctkAssertFatal(GctkCreateMesh(&MESH, VERTS, sizeof(VERTS), 6),
					GCTK_ERROR_GL_RUNTIME, "Failed to create mesh");
//	GctkAssertFatal(GctkCompileShader(&SHADER, VERT_SHADER, FRAG_SHADER),
//					GCTK_ERROR_GL_RUNTIME, "Failed to compile shader!");
	SHADER = *GctkSprite2DDefaultShader();
	SPRITE = GctkCreateSprite(GctkSprite2DDefaultShader(), &TEXTURE);

	while (GctkUpdate());

	GctkDeleteTexture(&TEXTURE);
	GctkDeleteSprite(&SPRITE);
	GctkDispose();


	return 0;
}