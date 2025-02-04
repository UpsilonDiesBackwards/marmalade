#version 430 core
layout (location = 0) out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D texture0;

void main() {
    FragColor = texture(texture0, texCoord);
}