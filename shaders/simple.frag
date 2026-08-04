#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform bool hasMatColor;
uniform vec3 matColor;

void main() {
    // render custom color
    if (hasMatColor) {
        FragColor = vec4(matColor, 1.0);
    } else {
        vec3 color = (FragPos / 3.0) + 0.1;
        FragColor = vec4(color, 1.0);
    }
}