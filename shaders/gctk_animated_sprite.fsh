#version 330 core

in vec2 UV;
out vec4 COLOR;

uniform sampler2DArray TEXTURE;
uniform vec4 COLOR_TINT = vec4(1.0);
uniform uint FRAME_INDEX = 0;

void main() {
    COLOR = texture(TEXTURE, vec3(UV, FRAME_INDEX)) * COLOR_TINT;
}