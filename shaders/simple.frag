#version 330 core

struct CustomMaterial {
    bool hasCustomColor;
    vec3 customColor;
};
struct Material {
    bool hasDiffuseColor;
    vec3 diffuseColor;
};

out vec4 FragColor;

in vec3 FragPos;

uniform CustomMaterial custom;
uniform Material material;

void main() {
    // render custom color
    if (custom.hasCustomColor) {
        FragColor = vec4(custom.customColor, 1.0);
    } else if (material.hasDiffuseColor) {
        FragColor = vec4(material.diffuseColor, 1.0);
    } else {
        vec3 color = (FragPos / 3.0) + 0.1;
        FragColor = vec4(color, 1.0);
    }
}