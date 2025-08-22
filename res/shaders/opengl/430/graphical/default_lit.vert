#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_enhanced_layouts : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (location = 0) out vec2 texCoord;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out mat3 TBN;

layout (location = 6) uniform vec3 viewPos;
layout (location = 7) uniform int numLights;

layout (location = 3) uniform mat4 projection;
layout (location = 4) uniform mat4 view;
layout (location = 5) uniform mat4 model;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    texCoord = aTexCoord;

    mat3 TBNMatrix = transpose(mat3(model));
    TBN = mat3(aTangent, aBitangent, Normal);

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
