#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 UV;

uniform mat4 VIEW_MATRIX;
uniform mat4 TRANSFORM_MATRIX;

void main() {
	gl_Position = VIEW_MATRIX * TRANSFORM_MATRIX * vec4(aPos, 1.0);
	UV = aUV;
}