#version 330 core

out vec4 FragColor;

in vec3 Normal;

void main() {
    vec3 norm = normalize(Normal);
    FragColor = vec4(0.8f, 0.3f, 0.8f, 1.0f);
    //FragColor = vec4(norm + 0.1f, 1.0f);
}