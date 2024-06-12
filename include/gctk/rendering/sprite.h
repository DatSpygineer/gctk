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
GCTK_API bool GctkSpriteDrawGeneric(const Sprite* sprite, Color color, Mat4 transform, Mat4 view);
#define GctkSpriteDraw(__sprite_ptr__, __color__, __transform_ptr__) GctkSpriteDrawGeneric(__sprite_ptr__, \
__color__, *((const Mat4*)(__transform_ptr__)), GctkGetViewportMatrix())

GCTK_API const Shader* GctkSprite2DDefaultShader();
GCTK_API void GctkSprite2DDeleteDefaultShader();

#endif