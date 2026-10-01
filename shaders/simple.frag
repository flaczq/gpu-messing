#version 330 core

struct CustomMaterial {
    bool hasCustomColor;
    vec3 customColor;
};

out vec4 FragColor;

in vec3 FragPos;

uniform CustomMaterial custom;

void main() {
    // render custom color
    if (custom.hasCustomColor) {
        FragColor = vec4(custom.customColor, 1.0);
    } else {
        vec3 color = (FragPos / 3.0) + 0.1;
        FragColor = vec4(color, 1.0);
    }
}