#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 matColor;

void main() {
    vec3 color = (FragPos / 3.0) + 0.1;
    // render custom color instead of black
    if (matColor != vec3(0.0)) {
        FragColor = vec4(matColor, 1.0);
    } else {
        FragColor = vec4(color, 1.0);
    }
}