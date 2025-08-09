#include "gctk_texture.hpp"

#include <cstring>
#include <fstream>

#include "gctk_debug.hpp"

namespace gctk {
	static constexpr uint8_t TEXTURE_IDENTIFIER[4] = { 'G', 'T', 'E', 'X' };

	struct TextureFlags {
		uint8_t target  : 3;
		uint8_t filter  : 1;
		uint8_t mipmaps : 1;
		uint8_t clamp_r : 1;
		uint8_t clamp_s : 1;
		uint8_t clamp_t : 1;
	};
	enum class TextureTarget {
		Texture1D           = 0b000,
		Texture1DArray      = 0b001,
		Texture2D           = 0b010,
		Texture2DArray      = 0b011,
		Texture3D           = 0b100,
		TextureCubeMap      = 0b101,
		TextureCubeMapArray = 0b110,
		Reserved            = 0b111
	};
	enum class TextureFormat {
		Grayscale,
		GrayscaleWithAlpha,
		Rgb,
		Rgba,
		Bgr,
		Bgra,

		SRgb,
		SRgbWithAlpha,

		Dxt1,
		Dxt1WithAlpha,
		Dxt3,
		Dxt5,

		Dxt1SRgb,
		Dxt1SRgbWithAlpha,
		Dxt3SRgb,
		Dxt5SRgb,

		BC6Signed,
		BC6Unsigned,
		BC7UNorm,

		BC7UNormSRgb
	};

	Texture::~Texture() {
		if (!m_bIsCopy && m_uId != 0) {
			glDeleteTextures(1, &m_uId);
			m_uId = 0;
		}
	}

	GLuint Texture::width() const {
		GLuint value;
		glGetTextureParameterIuiv(m_uId, GL_TEXTURE_WIDTH, &value);
		return value;
	}
	GLuint Texture::height() const {
		GLuint value;
		glGetTextureParameterIuiv(m_uId, GL_TEXTURE_HEIGHT, &value);
		return value;
	}
	GLuint Texture::depth() const {
		GLuint value;
		glGetTextureParameterIuiv(m_uId, GL_TEXTURE_DEPTH, &value);
		return value;
	}
	void Texture::apply() const {
		glBindTexture(m_uTarget, m_uId);
	}

	bool Texture::load(const std::string& path) {
		std::ifstream ifs(path, std::ios::binary); // TODO: Load file from an asset pack instead
		if (!ifs.is_open()) {
			LogErr("Could not load texture \"{}\": Failed to open file", path);
			glDeleteTextures(1, &m_uId);
			m_uId = 0;
			return false;
		}

		uint8_t identifier[4];
		ifs.read(reinterpret_cast<char*>(identifier), 4);
		if (memcmp(identifier, TEXTURE_IDENTIFIER, 4) != 0) {
			LogErr("Could not load texture \"{}\": Invalid identifier", path);
			glDeleteTextures(1, &m_uId);
			m_uId = 0;
			return false;
		}

		TextureFlags flags = { };
		ifs.read(reinterpret_cast<char*>(&flags), 1);

		uint16_t width, height, depth;
		ifs.read(reinterpret_cast<char*>(&width), 2);
		ifs.read(reinterpret_cast<char*>(&height), 2);
		ifs.read(reinterpret_cast<char*>(&depth), 2);

		uint8_t format;
		ifs.read(reinterpret_cast<char*>(&format), 1);

		auto pos = ifs.tellg();
		ifs.seekg(0, std::ios::end);
		auto data_size = ifs.tellg() - pos;
		ifs.seekg(pos, std::ios::beg);

		GLuint target;
		switch (static_cast<TextureTarget>(flags.target)) {
			case TextureTarget::Texture1D: target = GL_TEXTURE_1D; break;
			case TextureTarget::Texture1DArray: target = GL_TEXTURE_1D_ARRAY; break;
			case TextureTarget::Texture2D: target = GL_TEXTURE_2D; break;
			case TextureTarget::Texture2DArray: target = GL_TEXTURE_2D_ARRAY; break;
			case TextureTarget::Texture3D: target = GL_TEXTURE_3D; break;
			case TextureTarget::TextureCubeMap: target = GL_TEXTURE_CUBE_MAP; break;
			case TextureTarget::TextureCubeMapArray: target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
			default: {
				LogErr("Could not load texture \"{}\": Invalid texture target", path);
				glDeleteTextures(1, &m_uId);
				m_uId = 0;
				return false;
			}
		}

		if (target != m_uTarget) {
			LogErr("Could not load texture \"{}\": Unexpected target", path);
			glDeleteTextures(1, &m_uId);
			m_uId = 0;
			return false;
		}

		GLuint gl_format;
		GLint gl_internal_format;
		switch (static_cast<TextureFormat>(format)) {
			case TextureFormat::Grayscale: {
				gl_format = GL_RED;
				gl_internal_format = GL_RED;
			} break;
			case TextureFormat::GrayscaleWithAlpha: {
				gl_format = GL_RG;
				gl_internal_format = GL_RG;
			} break;
			case TextureFormat::Rgb: {
				gl_format = GL_RGB;
				gl_internal_format = GL_RGB;
			} break;
			case TextureFormat::Rgba: {
				gl_format = GL_RGBA;
				gl_internal_format = GL_RGBA;
			} break;
			case TextureFormat::SRgb: {
				gl_format = GL_SRGB;
				gl_internal_format = GL_SRGB;
			} break;
			case TextureFormat::SRgbWithAlpha: {
				gl_format = GL_SRGB_ALPHA;
				gl_internal_format = GL_SRGB_ALPHA;
			} break;

			case TextureFormat::Bgr: {
				gl_format = GL_BGR;
				gl_internal_format = GL_BGR;
			} break;
			case TextureFormat::Bgra: {
				gl_format = GL_BGRA;
				gl_internal_format = GL_BGRA;
			} break;

			case TextureFormat::Dxt1: {
				gl_format = GL_RGB;
				gl_internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			} break;
			case TextureFormat::Dxt1WithAlpha: {
				gl_format = GL_RGBA;
				gl_internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			} break;
			case TextureFormat::Dxt3: {
				gl_format = GL_RGBA;
				gl_internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			} break;
			case TextureFormat::Dxt5: {
				gl_format = GL_RGBA;
				gl_internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			} break;

			case TextureFormat::Dxt1SRgb: {
				gl_format = GL_SRGB;
				gl_internal_format = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			} break;
			case TextureFormat::Dxt1SRgbWithAlpha: {
				gl_format = GL_SRGB_ALPHA;
				gl_internal_format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			} break;
			case TextureFormat::Dxt3SRgb: {
				gl_format = GL_SRGB_ALPHA;
				gl_internal_format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			} break;
			case TextureFormat::Dxt5SRgb: {
				gl_format = GL_SRGB_ALPHA;
				gl_internal_format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			} break;

			case TextureFormat::BC6Signed: {
				gl_format = GL_RGB;
				gl_internal_format = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
			} break;
			case TextureFormat::BC6Unsigned: {
				gl_format = GL_RGB;
				gl_internal_format = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
			} break;
			case TextureFormat::BC7UNorm: {
				gl_format = GL_RGBA;
				gl_internal_format = GL_COMPRESSED_RGBA_BPTC_UNORM;
			} break;

			case TextureFormat::BC7UNormSRgb: {
				gl_format = GL_SRGB_ALPHA;
				gl_internal_format = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			} break;

			default: {
				LogErr("Could not load texture \"{}\": Invalid texture format", path);
				glDeleteTextures(1, &m_uId);
				m_uId = 0;
				return false;
			}
		}

		glBindTexture(m_uTarget, m_uId);

		glTextureParameteri(m_uId, GL_TEXTURE_MAG_FILTER, flags.filter ? GL_LINEAR : GL_NEAREST);
		glTextureParameteri(m_uId, GL_TEXTURE_MIN_FILTER,
			flags.mipmaps ?
			(flags.filter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR) :
			(flags.filter ? GL_LINEAR : GL_NEAREST)
		);

		glTextureParameteri(m_uId, GL_TEXTURE_WRAP_R, flags.clamp_r ? GL_CLAMP : GL_REPEAT);
		glTextureParameteri(m_uId, GL_TEXTURE_WRAP_S, flags.clamp_s ? GL_CLAMP : GL_REPEAT);
		glTextureParameteri(m_uId, GL_TEXTURE_WRAP_T, flags.clamp_t ? GL_CLAMP : GL_REPEAT);

		uint8_t* data = new uint8_t[data_size];
		ifs.read(reinterpret_cast<char*>(data), data_size);

		switch (m_uTarget) {
			case GL_TEXTURE_1D: {
				glTexImage1D(m_uTarget, 0, gl_internal_format, width, 0, gl_format, GL_UNSIGNED_BYTE, data);
			} break;
			case GL_TEXTURE_1D_ARRAY:
			case GL_TEXTURE_2D: {
				glTexImage2D(m_uTarget, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, data);
			} break;
			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_3D: {
				glTexImage3D(m_uTarget, 0, gl_internal_format, width, height, depth, 0, gl_format, GL_UNSIGNED_BYTE, data);
			} break;
			case GL_TEXTURE_CUBE_MAP: {
				glTexStorage2D(m_uTarget, 0, gl_internal_format, width, height);
				for (int i = 0; i < 6; i++) {
					glTexSubImage2D(m_uTarget, 0, height * i, 0, width, height, gl_format, GL_UNSIGNED_BYTE, data);
				}
			} break;
			case GL_TEXTURE_CUBE_MAP_ARRAY: {
				glTexStorage3D(m_uTarget, 0, gl_internal_format, width, height, depth);
				for (int j = 0; j < depth; j++) {
					for (int i = 0; i < 6; i++) {
						glTexSubImage3D(m_uTarget, 0, height * i, 0, j * 6, width, height, 1, gl_format, GL_UNSIGNED_BYTE, data);
					}
				}
			} break;
			default: /* Should never reach this case */ break;
		}

		if (flags.mipmaps) {
			glGenerateTextureMipmap(m_uId);
		}

		delete[] data;

		glBindTexture(m_uTarget, 0);

		return true;
	}
}
