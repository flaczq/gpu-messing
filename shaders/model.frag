#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D height;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // normals
	vec3 norm = normalize(Normal);

    // texture color
    vec4 texColor = texture(material.diffuse, TexCoords);

    // ambient (background light from far away)
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse (how much light stays on the object instead of bouncing off)
    vec3 lightDirNorm = normalize(-lightDir);
    // max to reject -values and leave only black (0,0,0) from behind the object
    // dot() = |a|*|b|*cos(angle) and less angle = more light
    float diff = max(dot(norm, lightDirNorm), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular (blinn-phong)
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDirNorm + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0f), 64.0f);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
    //FragColor = vec4(norm * 0.5 + 0.5, 1.0);
}