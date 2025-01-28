#version 430 core
layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

//#version 430 core
//layout (location = 0) in vec2 TexCoord;
//layout (location = 1) out vec4 FragColor;
//
//uniform sampler2D texture1;
//
//void main() {
//    FragColor = texture(texture1, TexCoord);
//}