#ifndef GCTK_SPRITE_H
#define GCTK_SPRITE_H

#include "gctk/rendering/texture.h"
#include "gctk/rendering/shader.h"
#include "gctk/rendering/mesh.h"
#include "gctk/common.h"

typedef struct Sprite {
	const Texture* texture;
	Mesh mesh;
} Sprite;

GCTK_API bool GctkCreateSprite(Sprite* sprite, const Shader* shader, const Texture* texture);
GCTK_API void GctkDeleteSprite(Sprite* sprite);
GCTK_API bool GctkDrawSpriteGeneric(const Sprite* sprite, Color color, Mat4 transform, Mat4 view);
#define GctkDrawSprite(__spr__, __color__, __transform__) \
	GctkDrawSpriteGeneric(__spr__, __color__, *((const Mat4*)(__transform__)), GctkGetViewportMatrix())

GCTK_API const Shader* GctkSprite2DDefaultShader();
GCTK_API void GctkSprite2DDeleteDefaultShader();

typedef struct Animation2D {
	const Texture* texture;
	size_t frame_count;
	Mesh mesh;
} Animation2D;

GCTK_API bool GctkCreateAnimation2D(Animation2D* anim, const Shader* shader, const Texture* texture);
GCTK_API void GctkDeleteAnimation2D(Animation2D* anim);
GCTK_API bool GctkDrawAnimation2D(const Animation2D* anim, uint32_t frame, Color color, Mat4 transform, Mat4 view);

typedef struct AnimatedSprite {
	Animation2D* animations;
	Animation2D* current_animation;
	size_t animation_count;
	size_t current_frame;
	size_t frame_rate, frame_time;
	bool loop;
} AnimatedSprite;

GCTK_API bool GctkCreateAnimatedSprite(AnimatedSprite* sprite,
											size_t frame_rate, bool looped,
											size_t default_anim, size_t animation_count, const Animation2D* animations);
GCTK_API bool GctkCreateAnimatedSpriteVA(AnimatedSprite* sprite,
											  size_t frame_rate, bool looped,
											  size_t default_anim, size_t animation_count, ...);
GCTK_API void GctkDeleteAnimatedSprite(AnimatedSprite* sprite);
GCTK_API void GctkDrawAnimatedSpriteGeneric(AnimatedSprite* sprite, Color color, Mat4 transform, Mat4 view);
#define GctkDrawAnimatedSprite(__sprite__, __color__, __transform__) \
	GctkDrawAnimatedSpriteGeneric(__sprite__, __color__, *((const Mat4*)(__transform__)), GctkGetViewportMatrix())

#endif