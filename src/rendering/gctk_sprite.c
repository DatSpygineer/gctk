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
		"uniform vec4 COLOR_TINT;\n"
		"\n"
		"void main() {\n"
		"\tCOLOR = texture(TEXTURE, UV) * COLOR_TINT;\n"
		"}\n";

static Shader GCTK_SPRITE2D_SHADER = GCTK_SHADER_NULL;

static float VERTS[] = {
	0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,   0.0f, 0.0f
};

extern Mat4 GctkGetViewportMatrix();

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

	glBindTexture(sprite->texture->target, 0);

	GctkDrawMesh(&sprite->mesh, transform, view);
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

bool GctkCreateAnimation2D(Animation2D* anim, const Shader* shader, const Texture* texture) {
	if (texture->target != GCTK_TEXTURE_2D_ARRAY) {
		GctkLogError(GCTK_ERROR_TEXTURE_INVALID_TARGET, "Failed to create sprite: Invalid texture target! Expected 2D array");
		return false;
	}

	Mesh mesh;
	if (!GctkCreateMesh(&mesh, VERTS, sizeof(VERTS), 6, shader)) return false;

	GLenum trg = GctkGetGLTarget(texture->target);
	glBindTexture(trg, texture->id);
	GLint depth;
	glGetTexParameteriv(trg, GL_TEXTURE_DEPTH, &depth);

	*anim = (Animation2D) {
		.texture = texture,
		.frame_count = depth,
		.mesh = mesh
	};
	return true;
}
void GctkDeleteAnimation2D(Animation2D* anim) {
	if (anim) {
		GctkDeleteMesh(&anim->mesh);
		memset(anim, 0, sizeof(Animation2D));
	}
}
bool GctkDrawAnimation2D(const Animation2D* anim, uint32_t frame, Color color, Mat4 transform, Mat4 view) {
	if (anim == NULL || anim->texture == NULL || anim->texture->id == 0) return false;

	GctkBindTexture(anim->texture);
	GctkApplyShader(anim->mesh.shader);
	GctkSetShaderUniformColor(anim->mesh.shader, "COLOR_TINT", color);
	GctkSetShaderUniformTexture(anim->mesh.shader, "TEXTURE", anim->texture);
	GctkSetShaderUniformUInt(anim->mesh.shader, "FRAME_INDEX", frame);

	glBindTexture(anim->texture->target, 0);

	GctkDrawMesh(&anim->mesh, transform, view);
	return true;
}

bool GctkCreateAnimatedSprite(AnimatedSprite* sprite,
	size_t frame_rate, bool looped,
	size_t default_anim, size_t animation_count, const Animation2D* animations) {
	if (sprite == NULL || animation_count == 0 || animations == NULL) {
		return false;
	}

	Animation2D* anims = (Animation2D*)malloc(sizeof(Animation2D) * animation_count);
	memcpy(anims, animations, sizeof(Animation2D) * animation_count);

	*sprite = (AnimatedSprite) {
		.animations = anims,
		.animation_count = animation_count,
		.current_animation = &anims[default_anim],
		.current_frame = 0,
		.frame_rate = frame_rate,
		.frame_time = 0,
		.loop =  looped
	};

	return true;
}
bool GctkCreateAnimatedSpriteVA(AnimatedSprite* sprite,
	size_t frame_rate, bool looped,
	size_t default_anim, size_t animation_count, ...) {
	if (sprite == NULL || animation_count == 0) {
		return false;
	}

	Animation2D* anims = (Animation2D*)malloc(sizeof(Animation2D) * animation_count); // NOLINT: "anims" pointer is stored in the sprite and freed on delete.
	va_list args;
	va_start(args, animation_count);
	for (size_t i = 0; i < animation_count; i++) {
		anims[i] = va_arg(args, Animation2D);
	}
	va_end(args);

	*sprite = (AnimatedSprite) {
		.animations = anims,
		.animation_count = animation_count,
		.current_animation = &anims[default_anim],
		.current_frame = 0,
		.frame_rate = frame_rate,
		.frame_time = 0,
		.loop =  looped
	};

	return true; // NOLINT: "anims" pointer is stored in the sprite and freed on delete.
}
void GctkDeleteAnimatedSprite(AnimatedSprite* sprite) {
	if (sprite && sprite->animations) {
		free(sprite->animations);
		memset(sprite, 0, sizeof(AnimatedSprite));
	}
}
void GctkDrawAnimatedSpriteGeneric(AnimatedSprite* sprite, Color color, Mat4 transform, Mat4 view) {
	if (sprite != NULL) {
		GctkDrawAnimation2D(sprite->current_animation, sprite->current_frame, color, transform, view);
		if (sprite->frame_time == sprite->frame_rate) {
			if (sprite->current_frame == sprite->current_animation->frame_count) {
				sprite->current_frame = sprite->loop ? 0 : sprite->current_frame;
			} else {
				sprite->current_frame++;
			}
			sprite->frame_time = 0;
		} else {
			sprite->frame_time++;
		}
	}
}