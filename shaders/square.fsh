#version 330 core

in vec2 UV;
out vec4 COLOR;

uniform float OutlineThreshold = 0.01;
uniform vec4 OutlineColor = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec2 UVScale = vec2(1.0);

float bool_to_float(bool value) {
	return value ? 1.0 : 0.0;
}

void main() {
	vec2 uv = fract(UV * UVScale);
	float outline = clamp(((uv.x <= OutlineThreshold ? 1.0 : 0.0) + (uv.x >= (1.0 - OutlineThreshold) ? 1.0 : 0.0)) +
					((uv.y <= OutlineThreshold ? 1.0 : 0.0) + (uv.y >= (1.0 - OutlineThreshold) ? 1.0 : 0.0)), 0.0, 1.0);
	COLOR = vec4(1.0, 1.0, 1.0, outline) * OutlineColor;
}