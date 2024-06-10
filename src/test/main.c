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
	-0.5f,  0.5f, 0.0f,	1.0f, 0.0f,
	 0.5f,  0.5f, 0.0f,	0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,	0.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,	0.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,	1.0f, 1.0f,
	-0.5f,  0.5f, 0.0f,	1.0f, 0.0f
};

static Mesh MESH;

static void render(double delta) {
	GctkBindTexture(&TEXTURE);
	GctkApplyShader(&SHADER);
	GctkSetShaderUniformTexture(&SHADER, "TEXTURE", &TEXTURE);

	GctkDrawMesh(&MESH);
}

int main(int argc, char** argv) {
	if (!GctkInit(argc, argv, "Test", "Gctk", VERSION(1, 0, 0, GCTK_VERSION_RELEASE))) {
		return GctkLastErrorCode();
	}

	GctkSetRenderCallback(&render);

	GctkAssertFatal(GctkLoadImageFromFile(&TEXTURE, "test.png", GCTK_IMAGE_POINT_FILTER),
					GCTK_ERROR_GL_RUNTIME, "Failed to load texture!");
	GctkAssertFatal(GctkCompileShader(&SHADER, VERT_SHADER, FRAG_SHADER),
					GCTK_ERROR_GL_RUNTIME, "Failed to create shader");
	GctkAssertFatal(GctkCreateMesh(&MESH, VERTS, sizeof(VERTS), 6),
					GCTK_ERROR_GL_RUNTIME, "Failed to create mesh");

	while (GctkUpdate());
	glDeleteTextures(1, &TEXTURE);

	GctkDispose();


	return 0;
}