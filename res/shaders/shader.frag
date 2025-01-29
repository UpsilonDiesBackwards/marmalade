#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) in vec2 TexCoord;

uniform sampler2D texture0;

void main() {
    FragColor = texture(texture0, TexCoord);
}