#version 330 core

struct Material {
	sampler2D diffuse;
};

out vec4 FragColor;

in vec2 TexCoords;

uniform Material material;

void main() {
    vec3 col = texture(material.diffuse, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
}