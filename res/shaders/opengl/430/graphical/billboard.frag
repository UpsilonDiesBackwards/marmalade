#version 430 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D icon;

void main() {
    vec4 tex = texture(icon, TexCoords);
    if (tex.a < 0.1) discard;
    FragColor = tex;
}