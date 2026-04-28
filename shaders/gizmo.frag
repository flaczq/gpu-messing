#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;

void main() {
    vec3 result = Color;
    FragColor = vec4(result, 1.0f);
}