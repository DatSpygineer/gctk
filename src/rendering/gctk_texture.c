#include "gctk/rendering/texture.h"
#include "gctk/filesys.h"
#include "gctk/debug.h"
#include "gctk/math.h"
#include "gctk/bithelper.h"
#include "gctk/collections.h"

#define STB_IMAGE_IMPLEMENTATION
#include "gctk/stb/stb_image.h"

GLuint GctkGetGLTarget(TextureTarget target) {
	switch (target) {
		case GCTK_TEXTURE_1D:				return GL_TEXTURE_1D;
		case GCTK_TEXTURE_2D:				return GL_TEXTURE_2D;
		case GCTK_TEXTURE_3D:				return GL_TEXTURE_3D;
		case GCTK_TEXTURE_CUBEMAP:			return GL_TEXTURE_CUBE_MAP;
		case GCTK_TEXTURE_1D_ARRAY: 		return GL_TEXTURE_1D_ARRAY;
		case GCTK_TEXTURE_2D_ARRAY: 		return GL_TEXTURE_2D_ARRAY;
		case GCTK_TEXTURE_CUBEMAP_ARRAY:	return GL_TEXTURE_CUBE_MAP_ARRAY;
	}
}

static bool GctkLoadGLTexture(Texture* texture, const uint8_t* data, size_t data_size,
							  int width, int height, int depth, ImageLoaderFlags flags, TextureTarget target,
							  TextureFormat format
) {
	if (texture == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Texture output pointer is NULL");
		return false;
	}
	if (data == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Texture data pointer is NULL");
		return false;
	}

	GctkGLCall(glGenTextures(1, &texture->id));
	if (texture->id == 0) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to generate GL texture");
		return false;
	}

	texture->target = target;
	texture->width = width;
	texture->height = height;
	texture->depth = depth;
	texture->format = format & ~GCTK_WITH_RLE;

	texture->clamp_r      = (flags & GCTK_IMAGE_FLAG_CLAMP_R) != 0;
	texture->clamp_s      = (flags & GCTK_IMAGE_FLAG_CLAMP_S) != 0;
	texture->clamp_t      = (flags & GCTK_IMAGE_FLAG_CLAMP_T) != 0;
	texture->point_filter = (flags & GCTK_IMAGE_FLAG_POINT_FILTER) != 0;
	texture->mipmaps      = (flags & GCTK_IMAGE_FLAG_GENERATE_MIPMAPS) != 0;

	GLenum gl_target = GctkGetGLTarget(target);
	GctkGLCall(glBindTexture(gl_target, texture->id));

	uint8_t* image_data = (uint8_t*)data;
	if (format >= GCTK_INDEXED_8 && format <= GCTK_INDEXED_16_ALPHA) {
		size_t offset = 0;
		uint16_t palette_size;

		if (format >= GCTK_INDEXED_16) {
			palette_size = *((const uint16_t*)data);
			offset += 2;
		} else {
			palette_size = *((const uint8_t*)data);
			offset++;
		}

		size_t palette_start = offset;
		offset += palette_size * 3;
		if (data_size - offset <= 0) {
			GctkGLCall(glDeleteTextures(1, &texture->id)); texture->id = 0;
			GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to read palette data");
			return false;
		}

		size_t pixel_count = width;
		if (target >= GCTK_TEXTURE_2D) {
			pixel_count *= height;
		}
		if (target == GCTK_TEXTURE_3D || target >= GCTK_TEXTURE_2D_ARRAY) {
			pixel_count *= depth;
		}

		const size_t pixel_size = (format == GCTK_INDEXED_8_ALPHA || format == GCTK_INDEXED_16_ALPHA) ? 4 : 3;
		image_data = (uint8_t*)malloc(pixel_count * pixel_size);
		memset(image_data, 0, pixel_count * pixel_size);

		size_t write_offset = 0;
		for (size_t i = 0; i < pixel_count; i++) {
			uint8_t alpha = (format == GCTK_INDEXED_8_ALPHA || format == GCTK_INDEXED_16_ALPHA) ? data[offset++] : 0xFF;
			uint16_t index = data[offset++];
			if (format == GCTK_INDEXED_16 || format == GCTK_INDEXED_16_ALPHA) {
				index |= data[offset++] << 8;
			}

			if (format == GCTK_INDEXED_8_ALPHA || format == GCTK_INDEXED_16_ALPHA) {
				image_data[write_offset++] = alpha;
			}

			image_data[write_offset++] = data[palette_start + (3 * index)];
			image_data[write_offset++] = data[palette_start + (3 * index) + 1];
			image_data[write_offset++] = data[palette_start + (3 * index) + 2];
		}
	}

	static bool result = true;
	result = true;

	GLuint gl_format;
	switch (format) {
		case GCTK_GRAYSCALE: {
			gl_format = GL_R;
		} break;
		case GCTK_GRAYSCALE_ALPHA: {
			gl_format = GL_RG;
		} break;
		case GCTK_INDEXED_8:
		case GCTK_INDEXED_16:
		case GCTK_RGB: {
			gl_format = GL_RGB;
		} break;
		case GCTK_INDEXED_8_ALPHA:
		case GCTK_INDEXED_16_ALPHA:
		case GCTK_RGBA: {
			gl_format = GL_RGBA;
		} break;
	}

	GctkGLCall(glTexParameteri(gl_target, GL_TEXTURE_WRAP_R, (flags & GCTK_IMAGE_FLAG_CLAMP_R) ? GL_CLAMP : GL_REPEAT));
	GctkGLCall(glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, (flags & GCTK_IMAGE_FLAG_CLAMP_S) ? GL_CLAMP : GL_REPEAT));
	GctkGLCall(glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, (flags & GCTK_IMAGE_FLAG_CLAMP_T) ? GL_CLAMP : GL_REPEAT));
	GctkGLCall(glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, (flags & GCTK_IMAGE_FLAG_POINT_FILTER) ? GL_NEAREST : GL_LINEAR));
	GctkGLCall(glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER,
					(flags & GCTK_IMAGE_FLAG_GENERATE_MIPMAPS) ?
					((flags & GCTK_IMAGE_FLAG_POINT_FILTER) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) :
					((flags & GCTK_IMAGE_FLAG_POINT_FILTER) ? GL_NEAREST : GL_LINEAR)
	));

	switch (target) {
		case GCTK_TEXTURE_1D: {
			GctkGLCall(glTexImage1D(gl_target, 0, (GLint)gl_format, width, 0, gl_format, GL_UNSIGNED_BYTE, image_data));
		} break;
		case GCTK_TEXTURE_2D: {
			GctkGLCall(glTexImage2D(gl_target, 0, (GLint)gl_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, image_data));
		} break;
		case GCTK_TEXTURE_3D: {
			GctkGLCall(glTexImage3D(gl_target, 0, (GLint)gl_format, width, height, depth, 0, gl_format, GL_UNSIGNED_BYTE, image_data));
		} break;
		case GCTK_TEXTURE_CUBEMAP: {
			GctkGLCall(glTexStorage2D(gl_target, 0, gl_format, width / 6, height));
			for (int i = 0; i < 6; i++) {
				GctkGLCall(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (width / 6) * i, 0, 0, width, height, gl_format,
								GL_UNSIGNED_BYTE, image_data));
			}
		} break;
		case GCTK_TEXTURE_1D_ARRAY: {
			GctkGLCall(glTexStorage2D(gl_target, 0, gl_format, width, height));
			GctkGLCall(glTexSubImage2D(gl_target, 0, 0, 0, width, height, gl_format, GL_UNSIGNED_BYTE, image_data));
		} break;
		case GCTK_TEXTURE_2D_ARRAY: {
			GctkGLCall(glTexStorage3D(gl_target, 0, gl_format, width, height, depth));
			GctkGLCall(glTexSubImage3D(gl_target, 0, 0, 0, 0, width, height, depth, gl_format, GL_UNSIGNED_BYTE, image_data));
		} break;
		case GCTK_TEXTURE_CUBEMAP_ARRAY: {
			GctkGLCall(glTexStorage3D(gl_target, 0, gl_format, width / 6, height, depth));
			for (int i = 0; i < 6; i++) {
				GctkGLCall(glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, (width / 6) * i, 0, 0, 0, width, height,
								depth, gl_format, GL_UNSIGNED_BYTE, image_data));
			}
		} break;
	}

	if (flags & GCTK_IMAGE_FLAG_GENERATE_MIPMAPS) {
		GctkGLCall(glGenerateMipmap(gl_target));
	}

	GctkGLCall(glBindTexture(gl_target, 0));

	if (format >= GCTK_INDEXED_8 && format <= GCTK_INDEXED_16_ALPHA) {
		free(image_data);
	}
	return result;
}

bool GctkLoadImage(Texture* texture, const uint8_t* data, size_t data_size, ImageLoaderFlags flags) {
	if (data == NULL || data_size <= 0) {
		return false;
	}

	int w, h, c;
	void* image_data = stbi_load_from_memory(data, (int)data_size, &w, &h, &c, 0);
	if (image_data == NULL) {
		return false;
	}

	TextureFormat format;
	switch (c) {
		case 1: format = GCTK_GRAYSCALE; break;
		case 2: format = GCTK_GRAYSCALE_ALPHA; break;
		case 3: format = GCTK_RGB; break;
		default: format = GCTK_RGBA; break;
	}

	bool result = GctkLoadGLTexture(texture, image_data, w * h * c, w, h, 0, flags, GCTK_TEXTURE_2D, format);
	stbi_image_free(image_data);

	return result;
}
bool GctkLoadImageFromFile(Texture* texture, const char* path, ImageLoaderFlags flags) {
	if (path == NULL) {
		return false;
	}

	int w, h, c;
	void* image_data = stbi_load(path, &w, &h, &c, 0);
	if (image_data == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to open file \"%s\"", path);
		return false;
	}

	TextureFormat format;
	switch (c) {
		case 1: format = GCTK_GRAYSCALE; break;
		case 2: format = GCTK_GRAYSCALE_ALPHA; break;
		case 3: format = GCTK_RGB; break;
		default: format = GCTK_RGBA; break;
	}

	bool result = GctkLoadGLTexture(texture, image_data, w * h * c, w, h, 0, flags, GCTK_TEXTURE_2D, format);
	stbi_image_free(image_data);

	return result;
}

bool GctkLoadTexture(Texture* texture, const uint8_t* data, size_t data_size) {
	if (texture == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Texture output pointer is NULL");
		return false;
	}
	if (data == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Texture data pointer is NULL");
		return false;
	}
	if (data_size < 8) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Texture data size is too small! Expected at least 8 bytes for header");
		return false;
	}

	size_t offset = 0;
	if (data[offset++] == 'G' && data[offset++] == 'T' && data[offset++] == 'E' && data[offset++] == 'X') {
		offset++; // uint8_t version = data[offset++]; <= Currently unused!

		uint8_t flags = data[offset++];
		TextureTarget target = (flags >> 5) & 0b111;
		flags &= 0b00011111;

		uint8_t format = data[offset++];

		int width = *((const uint16_t*)(data + offset)); offset += 2;
		int height = 0;
		int depth = 0;

		if (target >= GCTK_TEXTURE_2D) {
			height = *((const uint16_t*)(data + offset)); offset += 2;
		}
		if (target == GCTK_TEXTURE_3D || target >= GCTK_TEXTURE_2D_ARRAY) {
			depth = *((const uint16_t*)(data + offset)); offset += 2;
		}

		if (format & GCTK_WITH_RLE) {
			size_t packet_size;
			switch ((TextureFormat)(format & ~GCTK_WITH_RLE)) {
				case GCTK_INDEXED_8:
				case GCTK_GRAYSCALE: {
					packet_size = 2;	// count (1), data (1)
				} break;
				case GCTK_RGB:
				case GCTK_INDEXED_16_ALPHA:
				case GCTK_INDEXED_16:
				case GCTK_INDEXED_8_ALPHA:
				case GCTK_GRAYSCALE_ALPHA: {
										// RGB, IDX16A        => count (1), data (3)
					packet_size = 4;	// LA88, IDX8A, IDX16 => count (1), data (2), unused (1)
				} break;
				case GCTK_RGBA: {
					packet_size = 6;	// count (1), data (4), unused (1)
				} break;
			}

			uint8_t stride;
			switch ((TextureFormat) (format & ~GCTK_WITH_RLE)) {
				case GCTK_INDEXED_8:
				case GCTK_GRAYSCALE: {
					stride = 1;
				} break;
				case GCTK_INDEXED_16:
				case GCTK_INDEXED_8_ALPHA:
				case GCTK_GRAYSCALE_ALPHA: {
					stride = 2;
				} break;
				case GCTK_INDEXED_16_ALPHA:
				case GCTK_RGB: {
					stride = 3;
				} break;
				case GCTK_RGBA: {
					stride = 4;
				} break;
			}
			size_t uncompressed_data_size = (width * GctkMin(height, 1) * GctkMin(depth, 1)) * stride;
			uint16_t palette_size;
			if (format >= GCTK_INDEXED_16) {
				palette_size = *((const uint16_t*)data);
				offset += 2;
			} else {
				palette_size = *((const uint8_t*)data);
				offset++;
			}
			uint8_t* uncompressed_data = (uint8_t*)malloc(uncompressed_data_size);

			size_t write_offset = 0;
			uint64_t packet = 0;
			while (offset < data_size) {
				size_t current_packet_size = (palette_size > 0 && write_offset < palette_size) ? 4 : packet_size;
				memcpy(&packet, data + offset, current_packet_size);
				uint8_t count = packet & 0xFF;
				packet >>= 8;

				size_t current_stride = (palette_size > 0 && write_offset < palette_size) ? 4 : stride;
				for (uint8_t i = 0; i < count; i++) {
					for (size_t j = 0; j < current_stride; j++) {
						uncompressed_data[write_offset++] = packet & 0xFF;
						packet >>= 8;
					}
				}

				offset += current_packet_size;
			}
			bool result = GctkLoadGLTexture(texture, uncompressed_data, uncompressed_data_size, width, height, depth,
									 (ImageLoaderFlags) flags, target, format & ~GCTK_WITH_RLE);
			free(uncompressed_data);
			return result;
		} else {
			return GctkLoadGLTexture(texture, data + offset, data_size - offset, width, height, depth,
									 (ImageLoaderFlags) flags, target, format & ~GCTK_WITH_RLE);
		}
	} else {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Invalid identifier! Expected 'GTEX'");
		return false;
	}
}
bool GctkLoadTextureFromFile(Texture* texture, const char* path) {
	FILE* f = GctkOpenFile(path, GCTK_FILEMODE_READ, GCTK_FILE_BINARY | GCTK_FILE_OPEN);
	if (f == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to open file \"%s\"", path);
		return false;
	}

	bool result = true;
	size_t size = GctkFileSize(f);
	uint8_t* data = (uint8_t*)malloc(size);

	if (GctkFileRead(f, data, size, 1) >= size) {
		result = GctkLoadTexture(texture, data, size);
	} else {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to read file \"%s\"", path);
		result = false;
	}

	free(data);
	GctkCloseFile(f);
	return result;
}

static bool GctkWriteTextureBinaryWriter(BinaryWriter* writer, const Texture* texture, bool rle) {
	GctkBinaryWriterAppend_u8(writer, 'G');
	GctkBinaryWriterAppend_u8(writer, 'T');
	GctkBinaryWriterAppend_u8(writer, 'E');
	GctkBinaryWriterAppend_u8(writer, 'X');

	GctkBinaryWriterAppend_u8(writer, GTEX_VERSION);

	uint8_t flags = 0;
	flags |= texture->target & 0b111u;
	flags |= (texture->clamp_r & 1u) << 3;
	flags |= (texture->clamp_s & 1u) << 4;
	flags |= (texture->clamp_t & 1u) << 5;
	flags |= (texture->point_filter & 1u) << 6;
	flags |= (texture->mipmaps & 1u) << 7;

	GctkBinaryWriterAppend_u8(writer, flags);
	GctkBinaryWriterAppend_u8(writer, rle ? texture->format | GCTK_WITH_RLE : texture->format);
	GctkBinaryWriterAppend_u16(writer, texture->width);
	if (texture->target >= GCTK_TEXTURE_2D) {
		GctkBinaryWriterAppend_u16(writer, texture->height);
	}
	if (texture->target == GCTK_TEXTURE_3D || texture->target >= GCTK_TEXTURE_2D_ARRAY) {
		GctkBinaryWriterAppend_u16(writer, texture->depth);
	}

	GLenum format;
	const GLsizei pixel_count = GctkMin(texture->width, 1) * GctkMin(texture->height, 1) * GctkMin(texture->depth, 1);
	GLsizei pixel_size;
	switch (texture->format) {
		case GCTK_GRAYSCALE: {
			format = GL_RED;
			pixel_size = 1;
		} break;
		case GCTK_GRAYSCALE_ALPHA: {
			format = GL_RG;
			pixel_size = 2;
		} break;
		case GCTK_RGB:
		case GCTK_INDEXED_16:
		case GCTK_INDEXED_8: {
			format = GL_RGB;
			pixel_size = 3;
		} break;
		default:
		case GCTK_RGBA:
		case GCTK_INDEXED_16_ALPHA:
		case GCTK_INDEXED_8_ALPHA: {
			format = GL_RGBA;
			pixel_size = 4;
		} break;
	}

	uint8_t* image_data = (uint8_t*)malloc(pixel_count * pixel_size);
	GctkGLCall(glGetTextureImage(texture->id, 0, format, GL_UNSIGNED_BYTE, pixel_count * pixel_size, image_data));

	bool result;
	if (rle) {
		uint8_t count = 0;
		uint64_t data = UINT64_MAX;

		size_t packet_size;
		switch ((TextureFormat)(format & ~GCTK_WITH_RLE)) {
			case GCTK_INDEXED_8:
			case GCTK_GRAYSCALE: {
				packet_size = 2;	// count (1), data (1)
			} break;
			case GCTK_RGB:
			case GCTK_INDEXED_16_ALPHA:
			case GCTK_INDEXED_16:
			case GCTK_INDEXED_8_ALPHA:
			case GCTK_GRAYSCALE_ALPHA: {
									// RGB, IDX16A        => count (1), data (3)
				packet_size = 4;	// LA88, IDX8A, IDX16 => count (1), data (2), unused (1)
			} break;
			case GCTK_RGBA: {
				packet_size = 6;	// count (1), data (4), unused (1)
			} break;
		}

		Vector vec;
		GctkVectorAlloc(&vec, 0, GctkGetDefaultAllocator());
		for (GLsizei i = 0; i < pixel_count; i++) {
			if (data > UINT32_MAX) {
				data = 0;
				memcpy(&data, image_data + (i * pixel_size), pixel_size);
				count = 1;
			} else {
				uint32_t data_current = 0;
				memcpy(&data_current, image_data + (i * pixel_size), pixel_size);
				if (data == data_current) {
					count++;
				} else {
					uint64_t packet = (data << (8 * pixel_size)) | count;
					GctkVectorAddItem(&vec, &packet, packet_size);
					data = UINT64_MAX;
					count = 0;
				}
			}
		}
		if (data <= UINT32_MAX) {
			uint64_t packet = (data << (8 * pixel_size)) | count;
			GctkVectorAddItem(&vec, &packet, packet_size);
			data = UINT64_MAX;
			count = 0;
		}
		result = GctkBinaryWriterAppendBytes(writer, vec.data, vec.count);
	} else {
		result = GctkBinaryWriterAppendBytes(writer, image_data, pixel_count * pixel_size);
	}
	return result;
}

bool GctkWriteTexture(const Texture* texture, uint8_t* buffer, size_t buffer_max_size, bool rle) {
	Vector vec;
	BinaryWriter writer = GctkBinaryWriterNewFromVector(&vec, GCTK_LITTLE_ENDIAN);
	bool result = GctkWriteTextureBinaryWriter(&writer, texture, rle);

	if (result) {
		memcpy(buffer, vec.data, GctkMax(vec.count, buffer_max_size));
	}

	GctkBinaryWriterClose(&writer);
	return result;
}
bool GctkWriteTextureToFile(const Texture* texture, const char* path, bool rle) {
	FILE* f = GctkOpenFile(path, GCTK_FILEMODE_WRITE, GCTK_FILE_OPEN_OR_CREATE | GCTK_FILE_BINARY);
	if (f == NULL) {
		GctkLogError(GCTK_ERROR_LOAD_TEXTURE_FAILURE, "Failed to open file \"%s\"", path);
		return false;
	}
	BinaryWriter writer = GctkBinaryWriterNewFromFile(f, GCTK_LITTLE_ENDIAN);
	bool result = GctkWriteTextureBinaryWriter(&writer, texture, rle);
	GctkBinaryWriterClose(&writer);
	return result;
}

void GctkDeleteTexture(Texture* texture) {
	if (texture != NULL) {
		GctkGLCall(glDeleteTextures(1, &texture->id));
		memset(texture, 0, sizeof(Texture));
	}
}
void GctkBindTexture(const Texture* texture) {
	if (texture != NULL) {
		GctkGLCall(glBindTexture(GctkGetGLTarget(texture->target), texture->id));
	}
}

Vec2 GctkTextureSize(const Texture* texture) {
	return VEC2(texture->width, texture->height);
}
Vec3 GctkTextureSize3D(const Texture* texture) {
	return VEC3(texture->width, texture->height, texture->depth);
}