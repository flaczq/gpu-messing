#version 330 core

out vec4 FragColor;

uniform vec3 matColor;

void main() {
    FragColor = vec4(matColor, 1.0f);
}