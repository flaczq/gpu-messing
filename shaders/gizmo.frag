#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;

void main() {
    float brightness = (Color.r + Color.g + Color.b) / 3.0;
    FragColor = vec4(Color * (1.0 - brightness), 1.0);
}