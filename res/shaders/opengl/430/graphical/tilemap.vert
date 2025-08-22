#version 430

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

layout (location = 2) in vec2 iWorldPos;
layout (location = 3) in vec2 iUVOffset;
layout (location = 4) in vec2 iUVSize;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec2 worldPos = aPos + iWorldPos;
    TexCoord = iUVOffset + aUV * iUVSize;

    gl_Position = projection * view * model * vec4(worldPos, 0.0, 1.0);
}
