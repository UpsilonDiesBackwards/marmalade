#version 430 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 FragPos;

layout (location = 0) uniform bool useLighting;
layout (location = 6) uniform vec3 viewPos;

uniform sampler2D texture0;

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
    vec3 texColor = texture(texture0, texCoord).rgb;

    if (useLighting) {
        vec3 result = vec3(0.0);

        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        for (int i = 0; i < numLights; ++i) {
            Light light = lights[i];

            vec3 lightVec = light.position - FragPos;
            float distance = length(lightVec);

            if (distance < light.radius)
            continue;

            vec3 lightDir = normalize(lightVec);

            float ambientStrength = 0.1;
            vec3 ambient = ambientStrength * light.color;

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * (light.color * light.intensity);

            float specularStrength = 0.5;
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = specularStrength * spec * light.color;

            float distanceFactor = 1.0 - (distance / light.radius);
            distanceFactor = clamp(distanceFactor, 0.0, 1.0);
            float attenuation = 1.0 / (1.0 + light.attenuation * distance * distance);

            vec3 lightContribution = (ambient + diffuse + specular) * attenuation * distanceFactor;

            result += lightContribution;
        }

        FragColor = vec4(result * texColor, texture(texture0, texCoord).a);
    } else {
        FragColor = texture(texture0, texCoord);
    }
}
