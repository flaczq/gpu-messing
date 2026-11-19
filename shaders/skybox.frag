#version 330 core

struct Material {
	samplerCube cubemap;
};

out vec4 FragColor;

in vec3 TexDir;

uniform Material material;

void main() {
	vec4 cubemapColor = texture(material.cubemap, TexDir);
    FragColor = cubemapColor;
}