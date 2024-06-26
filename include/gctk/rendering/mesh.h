#ifndef GCTK_MESH_H
#define GCTK_MESH_H

#include "gctk/common.h"
#include "gctk/rendering/shader.h"

#include <GL/glew.h>

typedef struct Mesh {
	GLuint vbo, vao, ebo;
	GLsizei vertex_count;
	const Shader* shader;
} Mesh;

#define GCTK_MESH_NULL ((Mesh){ 0 })

GCTK_API bool GctkCreateMesh(Mesh* mesh, const float* buffer, size_t buffer_size, GLsizei vertex_count,
							 const Shader* shader);

GCTK_API bool GctkCreateMeshWithIndex(Mesh* mesh,
									  const float* buffer, size_t buffer_size,
									  const GLuint* indices, size_t index_count,
									  GLsizei vertex_count, const Shader* shader
);

GCTK_API void GctkDrawMesh(const Mesh* mesh, Mat4 transform, Mat4 view);
GCTK_API void GctkDeleteMesh(Mesh* mesh);

#endif