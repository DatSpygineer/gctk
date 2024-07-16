#ifndef GCTK_SHADER_H
#define GCTK_SHADER_H

#include "gctk/common.h"
#include "gctk/math.h"
#include "gctk/rendering/texture.h"

typedef struct Shader {
	GLuint id;
	bool deleted;
} Shader;

#define GCTK_SHADER_NULL ((Shader){ 0 })

GCTK_API bool GctkCompileShader(Shader* shader, const char* vert, const char* frag);
GCTK_API bool GctkCompileShaderFile(Shader* shader, const char* vert_path, const char* frag_path);
GCTK_API bool GctkLoadShaderSPRV(Shader* shader,
								 const uint8_t* vert_data, size_t vert_size, const char* vert_entry,
								 const uint8_t* frag_data, size_t frag_size, const char* frag_entry);
GCTK_API void GctkDeleteShader(Shader* shader);

GCTK_API bool GctkShaderHasUniform(const Shader* shader, const char* name);

GCTK_API bool GctkSetShaderUniformInt(const Shader* shader, const char* name, GLint value);
GCTK_API bool GctkSetShaderUniformInt2(const Shader* shader, const char* name, GLint x, GLint y);
GCTK_API bool GctkSetShaderUniformInt3(const Shader* shader, const char* name, GLint x, GLint y, GLint z);
GCTK_API bool GctkSetShaderUniformInt4(const Shader* shader, const char* name, GLint x, GLint y, GLint z, GLint w);
GCTK_API bool GctkSetShaderUniformUInt(const Shader* shader, const char* name, GLuint value);
GCTK_API bool GctkSetShaderUniformUInt2(const Shader* shader, const char* name, GLuint x, GLuint y);
GCTK_API bool GctkSetShaderUniformUInt3(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z);
GCTK_API bool GctkSetShaderUniformUInt4(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z, GLuint w);
GCTK_API bool GctkSetShaderUniformFloat(const Shader* shader, const char* name, float value);
GCTK_API bool GctkSetShaderUniformFloat2(const Shader* shader, const char* name, float x, float y);
GCTK_API bool GctkSetShaderUniformFloat3(const Shader* shader, const char* name, float x, float y, float z);
GCTK_API bool GctkSetShaderUniformFloat4(const Shader* shader, const char* name, float x, float y, float z, float w);
GCTK_API bool GctkSetShaderUniformDouble(const Shader* shader, const char* name, double value);
GCTK_API bool GctkSetShaderUniformDouble2(const Shader* shader, const char* name, double x, double y);
GCTK_API bool GctkSetShaderUniformDouble3(const Shader* shader, const char* name, double x, double y, double z);
GCTK_API bool GctkSetShaderUniformDouble4(const Shader* shader, const char* name, double x, double y, double z, double w);
GCTK_API bool GctkSetShaderUniformVec2(const Shader* shader, const char* name, Vec2 value);
GCTK_API bool GctkSetShaderUniformVec3(const Shader* shader, const char* name, Vec3 value);
GCTK_API bool GctkSetShaderUniformVec4(const Shader* shader, const char* name, Vec4 value);
GCTK_API bool GctkSetShaderUniformQuat(const Shader* shader, const char* name, Quat value);
GCTK_API bool GctkSetShaderUniformColor(const Shader* shader, const char* name, Color value);
GCTK_API bool GctkSetShaderUniformMat4(const Shader* shader, const char* name, Mat4 value);
GCTK_API bool GctkSetShaderUniformTransform2D(const Shader* shader, const char* name, const Transform2D* transform);
GCTK_API bool GctkSetShaderUniformTransform3D(const Shader* shader, const char* name, const Transform3D* transform);
GCTK_API bool GctkSetShaderUniformViewport2D(const Shader* shader, const char* name, const Viewport2D* viewport);
GCTK_API bool GctkSetShaderUniformViewport3D(const Shader* shader, const char* name, const Viewport3D* viewport);
GCTK_API bool GctkSetShaderUniformTexture(const Shader* shader, const char* name, const Texture* value);

GCTK_API bool GctkGetShaderUniformInt(const Shader* shader, const char* name, GLint* values);
GCTK_API bool GctkGetShaderUniformUInt(const Shader* shader, const char* name, GLuint* values);
GCTK_API bool GctkGetShaderUniformFloat(const Shader* shader, const char* name, float* values);
GCTK_API bool GctkGetShaderUniformDouble(const Shader* shader, const char* name, double* values);
GCTK_API bool GctkGetShaderUniformVec2(const Shader* shader, const char* name, Vec2* value);
GCTK_API bool GctkGetShaderUniformVec3(const Shader* shader, const char* name, Vec3* value);
GCTK_API bool GctkGetShaderUniformVec4(const Shader* shader, const char* name, Vec4* value);
GCTK_API bool GctkGetShaderUniformQuat(const Shader* shader, const char* name, Quat* value);
GCTK_API bool GctkGetShaderUniformColor(const Shader* shader, const char* name, Color* value);
GCTK_API bool GctkGetShaderUniformMat4(const Shader* shader, const char* name, Mat4* value);
GCTK_API bool GctkGetShaderUniformTexture(const Shader* shader, const char* name, Texture* value, TextureTarget target);

GCTK_API void GctkApplyShader(const Shader* shader);

typedef enum MaterialParameterType {
	GCTK_PARAM_UNDEFINED,
	GCTK_PARAM_INT,
	GCTK_PARAM_INT_2,
	GCTK_PARAM_INT_3,
	GCTK_PARAM_INT_4,
	GCTK_PARAM_UINT,
	GCTK_PARAM_UINT_2,
	GCTK_PARAM_UINT_3,
	GCTK_PARAM_UINT_4,
	GCTK_PARAM_FLOAT,
	GCTK_PARAM_FLOAT_2,
	GCTK_PARAM_FLOAT_3,
	GCTK_PARAM_FLOAT_4,
	GCTK_PARAM_DOUBLE,
	GCTK_PARAM_DOUBLE_2,
	GCTK_PARAM_DOUBLE_3,
	GCTK_PARAM_DOUBLE_4,
	GCTK_PARAM_MATRIX_4,
	GCTK_PARAM_TEXTURE,
} MaterialParameterType;

typedef struct MaterialParameter {
	uint32_t hash;
	char name[128];
	union {
		GLint  int_val;
		GLint  int_vec_val[4];
		GLuint uint_val;
		GLuint  uint_vec_val[4];
		float  flt_val;
		float   flt_vec_val[4];
		double dbl_val;
		double  dbl_vec_val[4];
		Mat4   mat_val;
		const Texture* tex_val;
	};
	MaterialParameterType type;
} MaterialParameter;

typedef struct Material Material;
struct Material {
	const Shader* shader;
	MaterialParameter* params;
	size_t param_count;
};

#endif