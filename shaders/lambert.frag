#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
	vec3 norm = normalize(Normal);
    
    vec4 texColor = vec4(0.3f, 0.8f, 0.3f, 1.0f); 
    
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;
  	
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, 1.0f);
    //FragColor = vec4(norm + 0.1f, 1.0f);
}