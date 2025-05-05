#version 430 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in mat3 TBN;

layout (location = 0) uniform bool useLighting;
layout (location = 6) uniform vec3 viewPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

uniform vec2 uvScale = vec2(1.0);

// Lighting
struct Light {
    vec3 position;
    float intensity;
    vec3 color;
    float radius;
    float attenuation;
};

#define MAX_LIGHTS 16
layout (location = 7) uniform int numLights;
layout (location = 8) uniform Light lights[MAX_LIGHTS];

void main() {
    vec2 scaledUV = texCoord * uvScale;

    vec3 texColor = texture(albedoMap, scaledUV).rgb;
    vec3 normalMapSample = texture(normalMap, scaledUV).rgb;
    vec3 normal = normalize(TBN * (normalMapSample * 2.0 - 1.0));

    if (useLighting) {
        vec3 result = vec3(1.0f);
        vec3 viewDir = normalize(viewPos - FragPos);

        for (int i = 0; i < numLights; ++i) {
            Light light = lights[i];

            vec3 lightVec = light.position - FragPos;
            float distance = length(lightVec);
            if (distance > light.radius) { continue; }

            vec3 lightDir = normalize(lightVec);

            float ambientStrength = 0.1f;
            vec3 ambient = ambientStrength * light.color;

            float diff = max(dot(normal, lightDir), 0.1f);
            vec3 diffuse = diff * (light.color * light.intensity);

            float specularStrength = 0.1f;
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
            vec3 specular = specularStrength * spec * light.color;

            float dist = 1.0f - (distance / light.radius);
            dist = clamp(dist, 0.0f, 1.0f);

            float attenuation = 1.0f / (1.0f + light.attenuation * distance * distance);

            vec3 lighting = (ambient + diffuse + spec) * attenuation * dist;

            result += lighting;
        }

        FragColor = vec4(result * texColor, texture(albedoMap, scaledUV).a);
    } else {
        FragColor = texture(albedoMap, scaledUV);
    }
}
