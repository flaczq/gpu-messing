#version 330 core

struct Material {
	sampler2D diffuse;
    bool hasDiffuseColor;
    vec3 diffuseColor;
};
struct CustomMaterial {
    bool hasCustomColor;
    vec3 customColor;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform Material material;
uniform CustomMaterial custom;

void main() {
	vec3 norm = normalize(Normal);
    
    vec4 texColor;
    if (material.hasDiffuseColor) {
        texColor = vec4(material.diffuseColor, 1.0f);
    } else if (custom.hasCustomColor) {
        texColor = vec4(custom.customColor, 1.0f);
    } else {
        texColor = texture(material.diffuse, TexCoords);
    }
    
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;
  	
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, 1.0f);
    //FragColor = vec4(norm + 0.1f, 1.0f);
}