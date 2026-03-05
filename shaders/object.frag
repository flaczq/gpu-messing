#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
    vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
	// normals
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

	// ambient
	vec3 diffTex = vec3(texture(material.diffuse, TexCoords));
    vec3 ambient = light.ambient * diffTex;

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * diffTex;

	// specular
	vec3 specTex = vec3(texture(material.specular, TexCoords));
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * specTex;
	
	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}