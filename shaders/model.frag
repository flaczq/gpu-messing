#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D material_texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // normals
	vec3 norm = normalize(Normal);

    // texture color
    vec4 texColor = texture(material_texture_diffuse1, TexCoords);

    // ambient (background light from far away)
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse (how much light stays on the object instead of bouncing off)
    vec3 lightDirNorm = normalize(-lightDir);
    // max to reject -values and leave only black (0,0,0) from behind the object
    // dot() = |a|*|b|*cos(angle) and less angle = more light
    float diff = max(dot(norm, lightDirNorm), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular (shininess)
    float specularStrength = 0.5f;
    uint shininessStrenght = 32u;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(norm, -lightDir);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininessStrenght);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}