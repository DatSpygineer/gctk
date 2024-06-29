#version 330 core

in vec2 UV;
out vec4 COLOR;

uniform sampler2D TEXTURE;
uniform vec4 COLOR_TINT = vec4(1.0);

void main() {
    COLOR = texture(TEXTURE, UV) * COLOR_TINT;
}