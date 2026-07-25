#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 matColor;

void main() {
    //vec3 color = (FragPos / 3.0f) + 0.5f;
    FragColor = vec4(matColor, 1.0f);
}