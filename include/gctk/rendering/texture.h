#ifndef GCTK_TEXTURE_H
#define GCTK_TEXTURE_H

#include "gctk/common.h"
#include "gctk/math.h"

#include <GL/glew.h>

#define GTEX_VERSION 0

typedef enum {
	GCTK_TEXTURE_1D				= 0b000,
	GCTK_TEXTURE_2D				= 0b001,
	GCTK_TEXTURE_3D				= 0b010,
	GCTK_TEXTURE_CUBEMAP 		= 0b011,
	GCTK_TEXTURE_1D_ARRAY		= 0b100,
	GCTK_TEXTURE_2D_ARRAY		= 0b101,
	GCTK_TEXTURE_CUBEMAP_ARRAY	= 0b111
} TextureTarget;

typedef enum {
	// Grayscale image format, 1 byte per pixel.
	// Uses Red channel internally.
	GCTK_GRAYSCALE,
	// Grayscale with alpha, 2 bytes per pixel.
	// Uses RG internally. Alpha will be located in the green channel!
	GCTK_GRAYSCALE_ALPHA,
	// Indexed format with 8-bit palette index. (1 byte per pixel)
	// Uses RGB internally.
	GCTK_INDEXED_8,
	// Indexed format with 8-bit palette index and 8-bit alpha. (2 bytes per pixel)
	// Uses RGBA internally.
	GCTK_INDEXED_8_ALPHA,
	// Indexed format with 16-bit palette index. (2 bytes per pixel)
	// Uses RGB internally.
	GCTK_INDEXED_16,
	// Indexed format with 16-bit palette index and 8-bit alpha. (3 bytes per pixel)
	// Uses RGBA internally.
	GCTK_INDEXED_16_ALPHA,
	// RGB format, 3 bytes per pixel, 1 byte per component.
	GCTK_RGB,
	// RGBA format, 4 bytes per pixel, 1 byte per component.
	GCTK_RGBA
} TextureFormat;

// Flag used to indicate that the format uses Run-length encoding.
#define GCTK_WITH_RLE 0x80

typedef enum ImageLoaderFlags {
	GCTK_IMAGE_FLAG_DEFAULT				= 0x00000000,
	GCTK_IMAGE_FLAG_CLAMP_R				= 0x00000001,
	GCTK_IMAGE_FLAG_CLAMP_S				= 0x00000010,
	GCTK_IMAGE_FLAG_CLAMP_T				= 0x00000100,
	GCTK_IMAGE_FLAG_POINT_FILTER		= 0b00001000,
	GCTK_IMAGE_FLAG_GENERATE_MIPMAPS	= 0b00010000,
	GCTK_IMAGE_FLAG_CLAMP				= GCTK_IMAGE_FLAG_CLAMP_R | GCTK_IMAGE_FLAG_CLAMP_T | GCTK_IMAGE_FLAG_CLAMP_S
} ImageLoaderFlags;

typedef struct Texture {
	GLuint id;
	TextureTarget target;
	TextureFormat format;
	uint16_t width, height, depth;
	bool clamp_r, clamp_s, clamp_t;
	bool point_filter, mipmaps;
} Texture;

GCTK_API GLuint GctkGetGLTarget(TextureTarget target);

GCTK_API bool GctkIsTextureValid(const Texture* texture);
GCTK_API bool GctkLoadImage(Texture* texture, const uint8_t* data, size_t data_size, ImageLoaderFlags flags);
GCTK_API bool GctkLoadImageStoreData(Texture* texture, const uint8_t* data, size_t data_size, ImageLoaderFlags flags, uint8_t** data_out, size_t* data_out_size);
GCTK_API bool GctkLoadImageFromFile(Texture* texture, const char* path, ImageLoaderFlags flags);
GCTK_API bool GctkLoadImageFromFileStoreData(Texture* texture, const char* path, ImageLoaderFlags flags, uint8_t** data_out, size_t* data_out_size);

GCTK_API bool GctkLoadImageStrip(Texture* texture, const uint8_t* data, size_t data_size, ImageLoaderFlags flags);
GCTK_API bool GctkLoadImageStripFromFile(Texture* texture, const char* path, ImageLoaderFlags flags);

GCTK_API bool GctkLoadTexture(Texture* texture, const uint8_t* data, size_t data_size);
GCTK_API bool GctkLoadTextureStoreData(Texture* texture, const uint8_t* data, size_t data_size, uint8_t** data_out, size_t* data_out_size);
GCTK_API bool GctkLoadTextureFromFile(Texture* texture, const char* path);
GCTK_API bool GctkLoadTextureFromFileStoreData(Texture* texture, const char* path, uint8_t** data_out, size_t* data_out_size);

GCTK_API size_t GctkWriteTexture(const char* path, const Texture* texture, const uint8_t* data, size_t data_size, uint8_t** data_out);
GCTK_API bool   GctkWriteTextureToFile(const char* path, const Texture* texture, const uint8_t* data, size_t data_size);

GCTK_API void GctkDeleteTexture(Texture* texture);
GCTK_API void GctkBindTexture(const Texture* texture);
GCTK_API void GctkUnbindTexuture(TextureTarget target);

GCTK_API Vec2 GctkTextureSize(const Texture* texture);
GCTK_API Vec3 GctkTextureSize3D(const Texture* texture);

#endif