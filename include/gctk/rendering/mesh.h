#ifndef GCTK_MESH_H
#define GCTK_MESH_H

#include "gctk/common.h"

#include <GL/glew.h>

typedef struct Mesh {
	GLuint vbo, vao, ebo;
	GLsizei vertex_count;
} Mesh;

GCTK_API bool GctkCreateMesh(Mesh* mesh, const float* buffer, size_t buffer_size, GLsizei vertex_count);

GCTK_API bool GctkCreateMeshWithIndex(Mesh* mesh,
									  const float* buffer, size_t buffer_size,
									  const GLuint* indices, size_t index_count,
									  GLsizei vertex_count
);

GCTK_API void GctkDrawMesh(const Mesh* mesh);

#endif