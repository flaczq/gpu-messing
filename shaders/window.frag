#version 330 core

struct Material {
	sampler2D diffuse;
};

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;

void main() {
	vec4 texColor = texture(material.diffuse, TexCoords);
	if (texColor.a < 0.1) {
	//	discard;
	}
    FragColor = texColor;
}