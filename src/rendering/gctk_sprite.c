#include <gctk/debug.h>

#include "gctk/rendering/sprite.h"

const char* GCTK_SPRITE2D_VERTEX =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aUV;\n"
		"out vec2 UV;\n"
		"\n"
		"uniform mat4 VIEW_MATRIX;\n"
		"uniform mat4 TRANSFORM_MATRIX;\n"
		"\n"
		"void main() {\n"
		"\tgl_Position = VIEW_MATRIX * TRANSFORM_MATRIX * vec4(aPos, 1.0);\n"
		"\tUV = aUV;\n"
		"}\n";

const char* GCTK_SPRITE2D_FRAGMENT =
		"#version 330 core\n"
		"\n"
		"in vec2 UV;\n"
		"out vec4 COLOR;\n"
		"uniform sampler2D TEXTURE;\n"
		"uniform vec4 COLOR_TINT = vec4(1.0);\n"
		"\n"
		"void main() {\n"
		"\tCOLOR = texture(TEXTURE, UV) * COLOR_TINT;\n"
		"}\n";

const char* GCTK_ANIM2D_VERTEX =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec2 aUV;\n"
	"out vec2 UV;\n"
	"\n"
	"uniform mat4 VIEW_MATRIX;\n"
	"uniform mat4 TRANSFORM_MATRIX;\n"
	"\n"
	"void main() {\n"
	"	gl_Position = VIEW_MATRIX * TRANSFORM_MATRIX * vec4(aPos, 1.0);\n"
	"	UV = aUV;\n"
	"}\n";

const char* GCTK_ANIM2D_FRAGMENT =
	"#version 330 core\n"
	"\n"
	"in vec2 UV;\n"
	"out vec4 COLOR;\n"
	"\n"
	"uniform sampler2DArray TEXTURE;\n"
	"uniform vec4 COLOR_TINT = vec4(1.0);\n"
	"uniform uint FRAME_INDEX = 0u;\n"
	"\n"
	"void main() {\n"
	"	COLOR = texture(TEXTURE, vec3(UV, float(FRAME_INDEX))) * COLOR_TINT;\n"
	"}\n";

static Shader GCTK_SPRITE2D_SHADER = GCTK_SHADER_NULL;
static Shader GCTK_ANIM2D_SHADER   = GCTK_SHADER_NULL;

static float VERTS[] = {
	0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,   0.0f, 0.0f
};

extern Mat4 GctkGetViewportMatrix();

bool GctkIsSpriteValid(const Sprite* sprite) {
	return sprite != NULL && sprite->mesh.vbo != 0 && sprite->mesh.vao != 0 && sprite->texture->id != 0;
}

bool GctkCreateSprite(Sprite* sprite, const Shader* shader, const Texture* texture) {
	if (texture->target != GCTK_TEXTURE_2D) {
		GctkLogError(GCTK_ERROR_TEXTURE_INVALID_TARGET, "Failed to create sprite: Invalid texture target! Expected 2D");
		return false;
	}

	Mesh mesh;
	if (!GctkCreateMesh(&mesh, VERTS, sizeof(VERTS), 6, shader)) return false;
	*sprite = (Sprite){ texture, mesh };
	return true;
}
void GctkDeleteSprite(Sprite* sprite) {
	if (sprite) {
		GctkDeleteMesh(&sprite->mesh);
		memset(sprite, 0, sizeof(Sprite));
	}
}

bool GctkDrawSpriteGeneric(const Sprite* sprite, Color color, Mat4 transform, Mat4 view) {
	if (sprite == NULL || sprite->texture == NULL || sprite->texture->id == 0) return false;

	GctkBindTexture(sprite->texture);
	GctkApplyShader(sprite->mesh.shader);
	GctkSetShaderUniformColor(sprite->mesh.shader, "COLOR_TINT", color);
	GctkSetShaderUniformTexture(sprite->mesh.shader, "TEXTURE", sprite->texture);

	GctkDrawMesh(&sprite->mesh, transform, view);

	glBindTexture(sprite->texture->target, 0);
	return true;
}

const Shader* GctkSprite2DDefaultShader() {
	if (GCTK_SPRITE2D_SHADER.id == 0) {
		if (GctkCompileShader(&GCTK_SPRITE2D_SHADER, GCTK_SPRITE2D_VERTEX, GCTK_SPRITE2D_FRAGMENT)) {
			return &GCTK_SPRITE2D_SHADER;
		}
		return NULL;
	}
	return &GCTK_SPRITE2D_SHADER;
}
void GctkSprite2DDeleteDefaultShader() {
	GctkDeleteShader(&GCTK_SPRITE2D_SHADER);
}