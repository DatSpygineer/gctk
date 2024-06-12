#include "gctk/rendering/mesh.h"

#include "gctk/debug.h"

bool GctkCreateMesh(Mesh* mesh, const float* buffer, size_t buffer_size, GLsizei vertex_count, const Shader* shader) {
	return GctkCreateMeshWithIndex(mesh, buffer, buffer_size, NULL, 0, vertex_count, shader);
}

bool GctkCreateMeshWithIndex(Mesh* mesh, const float* buffer, size_t buffer_size, const GLuint* indices,
							 size_t index_count, GLsizei vertex_count, const Shader* shader) {
	GLuint vbo, vao, ebo;

	GctkGLCall(glGenVertexArrays(1, &vao));
	if (vao == 0) {
		GctkGLCall(GctkLogError(GCTK_ERROR_GL_RUNTIME, "Failed to generate vao"));
		return false;
	}

	GctkGLCall(glBindVertexArray(vao));

	GctkGLCall(glGenBuffers(1, &vbo));
	if (vbo == 0) {
		GctkGLCall(glDeleteVertexArrays(1, &vao));
		GctkLogError(GCTK_ERROR_GL_RUNTIME, "Failed to generate vbo");
		return false;
	}

	GctkGLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GctkGLCall(glBufferData(GL_ARRAY_BUFFER, (GLsizei)buffer_size, buffer, GL_STATIC_DRAW));

	if (indices == NULL || index_count == 0) {
		ebo = 0;
	} else {
		GctkGLCall(glGenBuffers(1, &ebo));
		if (ebo == 0) {
			GctkGLCall(glDeleteVertexArrays(1, &vao));
			GctkGLCall(glDeleteBuffers(1, &vbo));
			GctkLogError(GCTK_ERROR_GL_RUNTIME, "Failed to generate ebo");
			return false;
		}

		GctkGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
		GctkGLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)index_count, indices, GL_STATIC_DRAW));
	}

	GctkGLCall(glEnableVertexAttribArray(0));
	GctkGLCall(glEnableVertexAttribArray(1));
	GctkGLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GctkGLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

	*mesh = (Mesh){ .vao = vao, .vbo = vbo, .ebo = ebo, .vertex_count = vertex_count, .shader = shader };
	return true;
}

void GctkDrawMesh(const Mesh* mesh, Mat4 transform, Mat4 view) {
	if (mesh != NULL && mesh->vao != 0) {
		GctkApplyShader(mesh->shader);
		GctkSetShaderUniformMat4(mesh->shader, "TRANSFORM_MATRIX", transform);
		GctkSetShaderUniformMat4(mesh->shader, "VIEW_MATRIX", view);
		GctkGLCall(glBindVertexArray(mesh->vao));
		if (mesh->ebo != 0) {
			GctkGLCall(glDrawElements(GL_TRIANGLES, mesh->vertex_count, GL_UNSIGNED_INT, 0));
		} else {
			GctkGLCall(glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count));
		}
	}
}

void GctkDeleteMesh(Mesh* mesh) {
	if (mesh) {
		if (mesh->vao) {
			glDeleteVertexArrays(1, &mesh->vao);
		}
		if (mesh->vbo) {
			glDeleteBuffers(1, &mesh->vbo);
		}
		if (mesh->ebo) {
			glDeleteBuffers(1, &mesh->ebo);
		}
		memset(mesh, 0, sizeof(Mesh));
	}
}
