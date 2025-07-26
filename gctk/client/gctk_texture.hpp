#pragma once

#include <string>

#include <GL/glew.h>

namespace gctk {
	class Texture {
	protected:
		GLuint m_uTarget;
		GLuint m_uId;
		bool m_bIsCopy;
	public:
		constexpr Texture(const GLuint id, const GLuint target, const bool is_copy) :
			m_uTarget(id), m_uId(target), m_bIsCopy(is_copy) { }
		explicit constexpr Texture(const GLuint target = GL_TEXTURE_2D) : Texture(0, target, false) {
			glGenTextures(1, &m_uId);
		}
		virtual ~Texture();

		[[nodiscard]] virtual bool load(const std::string& path);
		[[nodiscard]] GLuint width() const;
		[[nodiscard]] GLuint height() const;
		[[nodiscard]] GLuint depth() const;

		[[nodiscard]] constexpr GLuint id() const { return m_uId; }
		[[nodiscard]] constexpr GLuint target() const { return m_uTarget; }

		void apply() const;
	};

	class Texture1D final : public Texture {
	public:
		Texture1D() : Texture(GL_TEXTURE_1D) { }
		Texture1D(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_1D, is_copy) { }
	};
	class Texture1DArray final : public Texture {
	public:
		Texture1DArray() : Texture(GL_TEXTURE_1D_ARRAY) { }
		Texture1DArray(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_1D_ARRAY, is_copy) { }
	};
	class Texture2D final : public Texture {
	public:
		Texture2D() : Texture(GL_TEXTURE_2D) { }
		Texture2D(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_2D, is_copy) { }
	};
	class Texture2DArray final : public Texture {
	public:
		Texture2DArray() : Texture(GL_TEXTURE_2D_ARRAY) { }
		Texture2DArray(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_2D_ARRAY, is_copy) { }
	};
	class Texture3D final : public Texture {
	public:
		Texture3D() : Texture(GL_TEXTURE_3D) { }
		Texture3D(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_3D, is_copy) { }
	};
	class TextureCubeMap final : public Texture {
	public:
		TextureCubeMap() : Texture(GL_TEXTURE_CUBE_MAP) { }
		TextureCubeMap(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_CUBE_MAP, is_copy) { }
	};
	class TextureCubeMapArray final : public Texture {
	public:
		TextureCubeMapArray() : Texture(GL_TEXTURE_CUBE_MAP_ARRAY) { }
		TextureCubeMapArray(const GLuint id, const bool is_copy) : Texture(id, GL_TEXTURE_CUBE_MAP_ARRAY, is_copy) { }
	};
}