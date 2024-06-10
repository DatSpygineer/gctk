#include "gctk/rendering/shader.h"
#include "gctk/debug.h"
#include "gctk/str.h"

static bool GctkCompileSingleShader(GLuint* sid, const char* src, GLenum type, const char* debug_name,
									GctkDebugInfo debug_info) {
	*sid = GctkGLCall(glCreateShader(type));
	if (*sid == 0) {
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_COMPILE_SHADER, debug_info,
								 "Failed to compile %s shader: Could not generate shader id", debug_name
		);
		return false;
	}

	int success = 0;
	GctkGLCall(glShaderSource(*sid, 1, &src, NULL));
	GctkGLCall(glCompileShader(*sid));
	GctkGLCall(glGetShaderiv(*sid, GL_COMPILE_STATUS, &success));

	if (success == 0) {
		char info_log[1025] = { 0 };
		GctkGLCall(glGetShaderInfoLog(*sid, 1024, NULL, info_log));
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_COMPILE_SHADER, debug_info,
								 "Failed to compile %s shader: %s", debug_name, info_log
		);
		GctkGLCall(glDeleteShader(*sid));
		*sid = 0;
		return false;
	}

	return true;
}
static bool GctkLoadSingleShader(GLuint* sid, const uint8_t* data, size_t data_size, const char* entry,
								 GLenum type, const char* debug_name, GctkDebugInfo debug_info) {
	*sid = GctkGLCall(glCreateShader(type));
	if (*sid == 0) {
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_LOAD_SHADER, debug_info,
								 "Failed to compile %s shader: Could not generate shader id", debug_name
		);
		return false;
	}

	int success = 0;
	GctkGLCall(glShaderBinary(1, sid, GL_SHADER_BINARY_FORMAT_SPIR_V, data, (GLsizei)data_size));
	GctkGLCall(glGetShaderiv(*sid, GL_COMPILE_STATUS, &success));

	if (success == 0) {
		char info_log[1025] = { 0 };
		GctkGLCall(glGetShaderInfoLog(*sid, 1024, NULL, info_log));
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_COMPILE_SHADER, debug_info,
								 "Failed to load %s shader: %s", debug_name, info_log
		);
		GctkGLCall(glDeleteShader(*sid));
		*sid = 0;
		return false;
	}

	GctkGLCall(glSpecializeShader(*sid, entry, 0, NULL, NULL));
	return true;
}
static bool GctkLinkShader(GLuint* pid, GctkDebugInfo debug_info, const GLuint* shaders, size_t shader_count) {
	*pid = GctkGLCall(glCreateProgram());
	if (*pid == 0) {
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_LINK_SHADER, debug_info, "Failed to generate shader program");
		return false;
	}

	for (size_t i = 0; i < shader_count; i++) {
		GctkGLCall(glAttachShader(*pid, shaders[i]));
	}

	int success = 0;
	GctkGLCall(glLinkProgram(*pid));
	GctkGLCall(glGetProgramiv(*pid, GL_LINK_STATUS, &success));

	bool result = true;
	if (success == 0) {
		char info_log[1025] = { 0 };
		GctkGLCall(glGetProgramInfoLog(*pid, 1024, NULL, info_log));
		GctkDispatchDebugMessage(GCTK_MESSAGE_ERROR, GCTK_ERROR_LINK_SHADER, debug_info, "Failed to link shader: %s",
								 info_log
		);
		result = false;
	}

	for (size_t i = 0; i < shader_count; i++) {
		GctkGLCall(glDetachShader(*pid, shaders[i]));
		GctkGLCall(glDeleteShader(shaders[i]));
	}

	return result;
}

bool GctkCompileShader(Shader* shader, const char* vert, const char* frag) {
	GLuint shaders[2] = { 0 };
	if (!GctkCompileSingleShader(shaders, vert, GL_VERTEX_SHADER, "vertex", GCTK_GET_DEBUG_INFO)) {
		return false;
	}
	if (!GctkCompileSingleShader(shaders + 1, frag, GL_FRAGMENT_SHADER, "fragment", GCTK_GET_DEBUG_INFO)) {
		GctkGLCall(glDeleteShader(*shaders));
		return false;
	}

	shader->deleted = false;
	return GctkLinkShader(&shader->id, GCTK_GET_DEBUG_INFO, shaders, 2);
}
bool GctkLoadShaderSPRV(Shader* shader,
						const uint8_t* vert_data, size_t vert_size, const char* vert_entry,
						const uint8_t* frag_data, size_t frag_size, const char* frag_entry) {
	GLuint shaders[2] = { 0 };
	if (!GctkLoadSingleShader(shaders, vert_data, vert_size, vert_entry, GL_VERTEX_SHADER, "vertex",
							  GCTK_GET_DEBUG_INFO)) {
		return false;
	}
	if (!GctkLoadSingleShader(shaders + 1, frag_data, frag_size, frag_entry, GL_FRAGMENT_SHADER, "fragment",
							  GCTK_GET_DEBUG_INFO)) {
		GctkGLCall(glDeleteShader(*shaders));
		return false;
	}

	shader->deleted = false;
	return GctkLinkShader(&shader->id, GCTK_GET_DEBUG_INFO, shaders, 2);
}
void GctkDeleteShader(Shader* shader) {
	if (shader != NULL) {
		GctkGLCall(glDeleteProgram(shader->id));
		shader->id = 0;
		shader->deleted = true;
	}
}

bool GctkShaderHasUniform(const Shader* shader, const char* name) {
	return shader != NULL && GctkGLCall(glGetUniformLocation(shader->id, name) >= 0);
}

bool GctkSetShaderUniformInt(const Shader* shader, const char* name, GLint value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform1i(loc, value));
	return true;
}
bool GctkSetShaderUniformInt2(const Shader* shader, const char* name, GLint x, GLint y) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform2i(loc, x, y));
	return true;
}
bool GctkSetShaderUniformInt3(const Shader* shader, const char* name, GLint x, GLint y, GLint z) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform3i(loc, x, y, z));
	return true;
}
bool GctkSetShaderUniformInt4(const Shader* shader, const char* name, GLint x, GLint y, GLint z, GLint w) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4i(loc, x, y, z, w));
	return true;
}
bool GctkSetShaderUniformUInt(const Shader* shader, const char* name, GLuint value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform1ui(loc, value));
	return true;
}
bool GctkSetShaderUniformUInt2(const Shader* shader, const char* name, GLuint x, GLuint y) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform2ui(loc, x, y));
	return true;
}
bool GctkSetShaderUniformUInt3(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform3ui(loc, x, y, z));
	return true;
}
bool GctkSetShaderUniformUInt4(const Shader* shader, const char* name, GLuint x, GLuint y, GLuint z, GLuint w) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4ui(loc, x, y, z, w));
	return true;
}
bool GctkSetShaderUniformFloat(const Shader* shader, const char* name, float value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform1f(loc, value));
	return true;
}
bool GctkSetShaderUniformFloat2(const Shader* shader, const char* name, float x, float y) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform2f(loc, x, y));
	return true;
}
bool GctkSetShaderUniformFloat3(const Shader* shader, const char* name, float x, float y, float z) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform3f(loc, x, y, z));
	return true;
}
bool GctkSetShaderUniformFloat4(const Shader* shader, const char* name, float x, float y, float z, float w) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4f(loc, x, y, z, w));
	return true;
}
bool GctkSetShaderUniformDouble(const Shader* shader, const char* name, double value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform1d(loc, value));
	return true;
}
bool GctkSetShaderUniformDouble2(const Shader* shader, const char* name, double x, double y) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform2d(loc, x, y));
	return true;
}
bool GctkSetShaderUniformDouble3(const Shader* shader, const char* name, double x, double y, double z) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform3d(loc, x, y, z));
	return true;
}
bool GctkSetShaderUniformDouble4(const Shader* shader, const char* name, double x, double y, double z, double w) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4d(loc, x, y, z, w));
	return true;
}
bool GctkSetShaderUniformVec2(const Shader* shader, const char* name, Vec2 value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform2f(loc, value.x, value.y));
	return true;
}
bool GctkSetShaderUniformVec3(const Shader* shader, const char* name, Vec3 value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform3f(loc, value.x, value.y, value.z));
	return true;
}
bool GctkSetShaderUniformVec4(const Shader* shader, const char* name, Vec4 value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4f(loc, value.x, value.y, value.z, value.w));
	return true;
}
bool GctkSetShaderUniformQuat(const Shader* shader, const char* name, Quat value) {
	return GctkSetShaderUniformVec4(shader, name, value);
}
bool GctkSetShaderUniformColor(const Shader* shader, const char* name, Color value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform4f(loc, value.r, value.g, value.b, value.a));
	return true;
}
bool GctkSetShaderUniformMat4(const Shader* shader, const char* name, Mat4 value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniformMatrix4fv(loc, 1, GL_FALSE, value.items));
	return true;
}
bool GctkSetShaderUniformTransform2D(const Shader* shader, const char* name, const Transform2D* transform) {
	Transform2D transform_updated = *transform;
	Transform2DUpdateMatrix(&transform_updated);
	return GctkSetShaderUniformMat4(shader, name, transform_updated.matrix);
}
bool GctkSetShaderUniformTransform3D(const Shader* shader, const char* name, const Transform3D* transform) {
	Transform3D transform_updated = *transform;
	Transform3DUpdateMatrix(&transform_updated);
	return GctkSetShaderUniformMat4(shader, name, transform_updated.matrix);
}
bool GctkSetShaderUniformViewport2D(const Shader* shader, const char* name, const Viewport2D* viewport) {
	Viewport2D viewport_updated = *viewport;
	Viewport2DUpdateMatrix(&viewport_updated);
	return GctkSetShaderUniformMat4(shader, name, viewport_updated.matrix);
}
bool GctkSetShaderUniformViewport3D(const Shader* shader, const char* name, const Viewport3D* viewport) {
	Viewport3D viewport_updated = *viewport;
	Viewport3DUpdateMatrix(&viewport_updated);
	return GctkSetShaderUniformMat4(shader, name, viewport_updated.matrix);
}
bool GctkSetShaderUniformTexture(const Shader* shader, const char* name, const Texture* value) {
	if (shader == NULL) return false;

	GLint id = (GLint)((value == NULL) ? 0 : value->id) - 1;
	if (id < 0) {
		GctkLogError(GCTK_ERROR_GL_RUNTIME, "Attempt to assign null texture!");
		return false;
	}

	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glUniform1i(loc, id));
	return true;
}

bool GctkGetShaderUniformInt(const Shader* shader, const char* name, GLint* values) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformiv(shader->id, loc, values));
	return true;
}
bool GctkGetShaderUniformUInt(const Shader* shader, const char* name, GLuint* values) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformuiv(shader->id, loc, values));
	return true;
}
bool GctkGetShaderUniformFloat(const Shader* shader, const char* name, float* values) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, values));
	return true;
}
bool GctkGetShaderUniformDouble(const Shader* shader, const char* name, double* values) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformdv(shader->id, loc, values));
	return true;
}
bool GctkGetShaderUniformVec2(const Shader* shader, const char* name, Vec2* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformVec3(const Shader* shader, const char* name, Vec3* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformVec4(const Shader* shader, const char* name, Vec4* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformQuat(const Shader* shader, const char* name, Quat* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformColor(const Shader* shader, const char* name, Color* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformMat4(const Shader* shader, const char* name, Mat4* value) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GctkGLCall(glGetUniformfv(shader->id, loc, value->items));
	return true;
}
bool GctkGetShaderUniformTexture(const Shader* shader, const char* name, Texture* value, TextureTarget target) {
	if (shader == NULL) return false;
	GLint loc = GctkGLCall(glGetUniformLocation(shader->id, name));
	if (loc < 0) return false;

	GLenum gl_target = GctkGetGLTarget(target);
	GctkGLCall(glGetUniformuiv(shader->id, loc, &value->id));

	if (value->id == 0) return false;

	int w, h, d;
	glBindTexture(gl_target, value->id);

	glGetTexParameteriv(gl_target, GL_TEXTURE_WIDTH, &w);
	glGetTexParameteriv(gl_target, GL_TEXTURE_HEIGHT, &h);
	glGetTexParameteriv(gl_target, GL_TEXTURE_DEPTH, &d);

	glBindTexture(gl_target, 0);

	value->target = target;
	value->width = w;
	value->height = h;
	value->depth = d;

	return true;
}

void GctkApplyShader(const Shader* shader) {
	if (shader != NULL) {
		GctkGLCall(glUseProgram(!shader->deleted ? shader->id : 0));
	}
}

Material GctkCreateMaterial(const Shader* shader, size_t param_count) {
	Material mat = { .shader = shader, .param_count = param_count };
	if (param_count > 0) {
		mat.params = (MaterialParameter*)malloc(param_count * sizeof(MaterialParameter));
	} else {
		mat.params = NULL;
	}
	return mat;
}

static bool GctkSetMaterialParam(Material* material, const char* name, MaterialParameter param) {
	if (material == NULL || name == NULL) return false;

	param.hash = GctkStrHash(name);
	memset(param.name, 0, 128);
	memcpy(param.name, name, strlen(name));
	if (material->params != NULL) {
		for (size_t i = 0; i < material->param_count; i++) {
			if (material->params[i].hash == param.hash) {
				material->params[i] = param;
				return true;
			}
		}
		material->params = (MaterialParameter*)realloc(material->params, (material->param_count + 1) * sizeof(MaterialParameter));
		if (material->params != NULL) {
			material->params[material->param_count++] = param;
			return true;
		}
	} else {
		material->params = (MaterialParameter*)malloc(sizeof(MaterialParameter));
		if (material->params != NULL) {
			material->params[0] = param;
			material->param_count = 1;
			return true;
		}
	}
	return false;
}
static bool GctkGetMaterialParam(const Material* material, const char* name, MaterialParameter* param) {
	if (material == NULL || name == NULL) return false;

	hash_t hash = GctkStrHash(name);
	for (size_t i = 0; i < material->param_count; i++) {
		if (material->params[i].hash != hash) {
			*param = material->params[i];
			return true;
		}
	}

	return false;
}

bool GctkSetMaterialParamInt (Material* material, const char* name, GLint value) {
	return GctkSetMaterialParam(material, name, (MaterialParameter){ .int_val = value, .type = GCTK_PARAM_INT });
}
bool GctkSetMaterialParamInt2(Material* material, const char* name, const GLint value[2]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_INT_2;
	param.int_vec_val[0] = value[0];
	param.int_vec_val[1] = value[1];
	param.int_vec_val[2] = 0;
	param.int_vec_val[3] = 0;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamInt3(Material* material, const char* name, const GLint value[3]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_INT_3;
	param.int_vec_val[0] = value[0];
	param.int_vec_val[1] = value[1];
	param.int_vec_val[2] = value[2];
	param.int_vec_val[3] = 0;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamInt4(Material* material, const char* name, const GLint value[4]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_INT_4;
	param.int_vec_val[0] = value[0];
	param.int_vec_val[1] = value[1];
	param.int_vec_val[2] = value[2];
	param.int_vec_val[3] = value[3];
	return GctkSetMaterialParam(material, name, param);
}

bool GctkSetMaterialParamUInt (Material* material, const char* name, GLuint value) {
	return GctkSetMaterialParam(material, name, (MaterialParameter){ .uint_val = value, .type = GCTK_PARAM_UINT });
}
bool GctkSetMaterialParamUInt2(Material* material, const char* name, const GLuint value[2]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_UINT_2;
	param.uint_vec_val[0] = value[0];
	param.uint_vec_val[1] = value[1];
	param.uint_vec_val[2] = 0;
	param.uint_vec_val[3] = 0;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamUInt3(Material* material, const char* name, const GLuint value[3]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_UINT_3;
	param.uint_vec_val[0] = value[0];
	param.uint_vec_val[1] = value[1];
	param.uint_vec_val[2] = value[2];
	param.uint_vec_val[3] = 0;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamUInt4(Material* material, const char* name, const GLuint value[4]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_UINT_4;
	param.uint_vec_val[0] = value[0];
	param.uint_vec_val[1] = value[1];
	param.uint_vec_val[2] = value[2];
	param.uint_vec_val[3] = value[3];
	return GctkSetMaterialParam(material, name, param);
}

bool GctkSetMaterialParamFloat (Material* material, const char* name, float value) {
	return GctkSetMaterialParam(material, name, (MaterialParameter){ .flt_val = value, .type = GCTK_PARAM_FLOAT });
}
bool GctkSetMaterialParamFloat2(Material* material, const char* name, const float value[2]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_FLOAT_2;
	param.flt_vec_val[0] = value[0];
	param.flt_vec_val[1] = value[1];
	param.flt_vec_val[2] = 0.0f;
	param.flt_vec_val[3] = 0.0f;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamFloat3(Material* material, const char* name, const float value[3]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_FLOAT_3;
	param.flt_vec_val[0] = value[0];
	param.flt_vec_val[1] = value[1];
	param.flt_vec_val[2] = value[2];
	param.flt_vec_val[3] = 0.0f;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamFloat4(Material* material, const char* name, const float value[4]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_FLOAT_4;
	param.flt_vec_val[0] = value[0];
	param.flt_vec_val[1] = value[1];
	param.flt_vec_val[2] = value[2];
	param.flt_vec_val[3] = value[3];
	return GctkSetMaterialParam(material, name, param);
}

bool GctkSetMaterialParamDouble (Material* material, const char* name, double value) {
	return GctkSetMaterialParam(material, name, (MaterialParameter){ .dbl_val = value, .type = GCTK_PARAM_DOUBLE });
}
bool GctkSetMaterialParamDouble2(Material* material, const char* name, const double value[2]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_DOUBLE_2;
	param.dbl_vec_val[0] = value[0];
	param.dbl_vec_val[1] = value[1];
	param.dbl_vec_val[2] = 0.0f;
	param.dbl_vec_val[3] = 0.0f;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamDouble3(Material* material, const char* name, const double value[3]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_DOUBLE_3;
	param.dbl_vec_val[0] = value[0];
	param.dbl_vec_val[1] = value[1];
	param.dbl_vec_val[2] = value[2];
	param.dbl_vec_val[3] = 0.0;
	return GctkSetMaterialParam(material, name, param);
}
bool GctkSetMaterialParamDouble4(Material* material, const char* name, const double value[4]) {
	MaterialParameter param;
	param.type = GCTK_PARAM_DOUBLE_4;
	param.dbl_vec_val[0] = value[0];
	param.dbl_vec_val[1] = value[1];
	param.dbl_vec_val[2] = value[2];
	param.dbl_vec_val[3] = value[3];
	return GctkSetMaterialParam(material, name, param);
}

bool GctkSetMaterialParamMat4(Material* material, const char* name, Mat4 value) {
	MaterialParameter param;
	param.type = GCTK_PARAM_MATRIX_4;
	param.mat_val = value;
	return GctkSetMaterialParam(material, name, param);
}

bool GctkSetMaterialParamTexture(Material* material, const char* name, const Texture* value) {
	MaterialParameter param;
	param.type = GCTK_PARAM_TEXTURE;
	param.tex_val = value;
	return GctkSetMaterialParam(material, name, param);
}

bool GctkGetMaterialParamInt (const Material* material, const char* name, GLint* value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.int_val;
		return true;
	}

	return false;
}
bool GctkGetMaterialParamInt2(const Material* material, const char* name, GLint value[2]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.int_vec_val[0];
		value[1] = param.int_vec_val[1];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamInt3(const Material* material, const char* name, GLint value[3]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.int_vec_val[0];
		value[1] = param.int_vec_val[1];
		value[2] = param.int_vec_val[2];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamInt4(const Material* material, const char* name, GLint value[4]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.int_vec_val[0];
		value[1] = param.int_vec_val[1];
		value[2] = param.int_vec_val[2];
		value[3] = param.int_vec_val[3];
		return true;
	}

	return false;
}

bool GctkGetMaterialParamUInt (const Material* material, const char* name, GLuint* value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.uint_val;
		return true;
	}

	return false;
}
bool GctkGetMaterialParamUInt2(const Material* material, const char* name, GLuint value[2]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.uint_vec_val[0];
		value[1] = param.uint_vec_val[1];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamUInt3(const Material* material, const char* name, GLuint value[3]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.uint_vec_val[0];
		value[1] = param.uint_vec_val[1];
		value[2] = param.uint_vec_val[2];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamUInt4(const Material* material, const char* name, GLuint value[4]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.uint_vec_val[0];
		value[1] = param.uint_vec_val[1];
		value[2] = param.uint_vec_val[2];
		value[3] = param.uint_vec_val[3];
		return true;
	}

	return false;
}

bool GctkGetMaterialParamFloat (const Material* material, const char* name, float* value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.flt_val;
		return true;
	}

	return false;
}
bool GctkGetMaterialParamFloat2(const Material* material, const char* name, float value[2]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.flt_vec_val[0];
		value[1] = param.flt_vec_val[1];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamFloat3(const Material* material, const char* name, float value[3]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.flt_vec_val[0];
		value[1] = param.flt_vec_val[1];
		value[2] = param.flt_vec_val[2];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamFloat4(const Material* material, const char* name, float value[4]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.flt_vec_val[0];
		value[1] = param.flt_vec_val[1];
		value[2] = param.flt_vec_val[2];
		value[3] = param.flt_vec_val[3];
		return true;
	}

	return false;
}

bool GctkGetMaterialParamDouble (const Material* material, const char* name, double* value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.dbl_val;
		return true;
	}

	return false;
}
bool GctkGetMaterialParamDouble2(const Material* material, const char* name, double value[2]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.dbl_vec_val[0];
		value[1] = param.dbl_vec_val[1];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamDouble3(const Material* material, const char* name, double value[3]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.dbl_vec_val[0];
		value[1] = param.dbl_vec_val[1];
		value[2] = param.dbl_vec_val[2];
		return true;
	}

	return false;
}
bool GctkGetMaterialParamDouble4(const Material* material, const char* name, double value[4]) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		value[0] = param.dbl_vec_val[0];
		value[1] = param.dbl_vec_val[1];
		value[2] = param.dbl_vec_val[2];
		value[3] = param.dbl_vec_val[3];
		return true;
	}

	return false;
}

bool GctkGetMaterialParamMat4(const Material* material, const char* name, Mat4* value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.mat_val;
		return true;
	}

	return false;
}
bool GctkGetMaterialParamTexture(const Material* material, const char* name, const Texture** value) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		*value = param.tex_val;
		return true;
	}

	return false;
}

MaterialParameterType GctkGetMaterialParamType(const Material* material, const char* name) {
	MaterialParameter param;
	if (GctkGetMaterialParam(material, name, &param)) {
		return param.type;
	}

	return GCTK_PARAM_UNDEFINED;
}

static bool GctkSetMaterialParamAsUniform(const Shader* shader, const MaterialParameter* param) {
	if (shader != NULL && param != NULL && param->type != GCTK_PARAM_UNDEFINED) {
		switch (param->type) {
			case GCTK_PARAM_INT: {
				return GctkSetShaderUniformInt(shader, param->name, param->int_val);
			}
			case GCTK_PARAM_INT_2: {
				return GctkSetShaderUniformInt2(shader, param->name, param->int_vec_val[0], param->int_vec_val[1]);
			}
			case GCTK_PARAM_INT_3: {
				return GctkSetShaderUniformInt3(shader, param->name, param->int_vec_val[0], param->int_vec_val[1],
										 					  param->int_vec_val[2]);
			}
			case GCTK_PARAM_INT_4: {
				return GctkSetShaderUniformInt4(shader, param->name, param->int_vec_val[0], param->int_vec_val[1],
										 param->int_vec_val[2], param->int_vec_val[3]);
			}
			case GCTK_PARAM_UINT: {
				return GctkSetShaderUniformUInt(shader, param->name, param->uint_val);
			}
			case GCTK_PARAM_UINT_2: {
				return GctkSetShaderUniformUInt2(shader, param->name, param->uint_vec_val[0], param->uint_vec_val[1]);
			}
			case GCTK_PARAM_UINT_3: {
				return GctkSetShaderUniformUInt3(shader, param->name, param->uint_vec_val[0], param->uint_vec_val[1],
																	  param->uint_vec_val[2]);
			}
			case GCTK_PARAM_UINT_4: {
				return GctkSetShaderUniformUInt4(shader, param->name, param->uint_vec_val[0], param->uint_vec_val[1],
																	  param->uint_vec_val[2], param->uint_vec_val[3]);
			}
			case GCTK_PARAM_FLOAT: {
				return GctkSetShaderUniformFloat(shader, param->name, param->flt_val);
			}
			case GCTK_PARAM_FLOAT_2: {
				return GctkSetShaderUniformFloat2(shader, param->name, param->flt_vec_val[0], param->flt_vec_val[1]);
			}
			case GCTK_PARAM_FLOAT_3: {
				return GctkSetShaderUniformFloat3(shader, param->name, param->flt_vec_val[0], param->flt_vec_val[1],
																	   param->flt_vec_val[2]);
			}
			case GCTK_PARAM_FLOAT_4: {
				return GctkSetShaderUniformFloat4(shader, param->name, param->flt_vec_val[0], param->flt_vec_val[1],
																	   param->flt_vec_val[2], param->flt_vec_val[3]);
			}
			case GCTK_PARAM_DOUBLE: {
				return GctkSetShaderUniformDouble(shader, param->name, param->dbl_val);
			}
			case GCTK_PARAM_DOUBLE_2: {
				return GctkSetShaderUniformDouble2(shader, param->name, param->dbl_vec_val[0], param->dbl_vec_val[1]);
			}
			case GCTK_PARAM_DOUBLE_3: {
				return GctkSetShaderUniformDouble3(shader, param->name, param->dbl_vec_val[0], param->dbl_vec_val[1],
																		param->dbl_vec_val[2]);
			}
			case GCTK_PARAM_DOUBLE_4: {
				return GctkSetShaderUniformDouble4(shader, param->name, param->dbl_vec_val[0], param->dbl_vec_val[1],
																		param->dbl_vec_val[2], param->dbl_vec_val[3]);
			}
			case GCTK_PARAM_MATRIX_4: {
				return GctkSetShaderUniformMat4(shader, param->name, param->mat_val);
			}
			case GCTK_PARAM_TEXTURE: {
				return GctkSetShaderUniformTexture(shader, param->name, param->tex_val);
			}
			default: /* Invalid type, fall through */ break;
		}
	}
	return false;
}

void GctkApplyMaterial(const Material* material) {
	if (material != NULL) {
		for (size_t i = 0; i < material->param_count; i++) {

		}
		GctkApplyShader(material->shader);
	}
}
void GctkDeleteMaterial(Material* material) {
	if (material && material->params) {
		free(material->params);
		memset(material, 0, sizeof(Material));
	}
}