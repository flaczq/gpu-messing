#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D material_texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    // normals
	vec3 norm = normalize(Normal);

    // texture color
    vec4 texColor = texture(material_texture_diffuse1, TexCoords);

    // ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    // change direction towards the light
    vec3 lightDirNorm = normalize(-lightDir);
    // less angle = more light
    float diff = max(dot(norm, lightDirNorm), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}