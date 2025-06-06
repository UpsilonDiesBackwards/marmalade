#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 pos;

out vec2 TexCoords;

void main() {
    mat3 camRot = transpose(mat3(view));

    vec3 worldPos = pos + camRot * aPos;

    gl_Position = proj * view * vec4(worldPos, 1.0);
    TexCoords = aTex;
}