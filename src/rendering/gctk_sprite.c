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
		"uniform vec4 COLOR_TINT;\n"
		"\n"
		"void main() {\n"
		"\tCOLOR = texture(TEXTURE, UV) * COLOR_TINT;\n"
		"}\n";

static Shader GCTK_SPRITE2D_SHADER = GCTK_SHADER_NULL;
static Mesh GCTK_SPRITE2D_MESH = GCTK_MESH_NULL;

static float VERTS[] = {
	-0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
	-0.5f,  0.5f, 0.0f,   0.0f, 0.0f
};

extern Mat4 GctkGetViewportMatrix();

Sprite GctkCreateSprite(const Shader* shader, const Texture* texture) {
	GctkCreateMesh(&GCTK_SPRITE2D_MESH, VERTS, sizeof(VERTS), 6);
	return (Sprite){shader, texture, GCTK_SPRITE2D_MESH };
}
void GctkDeleteSprite(Sprite* sprite) {
	if (sprite) {
		GctkDeleteMesh(&sprite->mesh);
	}
}

bool GctkSpriteDrawGeneric(const Sprite* sprite, Color color, Mat4 transform, Mat4 view) {
	if (sprite == NULL || sprite->texture->id == 0) return false;

	GctkBindTexture(sprite->texture);
	GctkApplyShader(sprite->shader);
	GctkSetShaderUniformMat4(sprite->shader, "TRANSFORM_MATRIX", transform);
	GctkSetShaderUniformMat4(sprite->shader, "VIEW_MATRIX", view);
	GctkSetShaderUniformColor(sprite->shader, "COLOR_TINT", color);
	GctkSetShaderUniformTexture(sprite->shader, "TEXTURE", sprite->texture);

	glBindTexture(sprite->texture->target, 0);

	GctkDrawMesh(&sprite->mesh);
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