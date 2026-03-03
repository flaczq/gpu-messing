#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float interpolate;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
	//vec2 flippedCoord = 4*vec2(1.0 - TexCoord.x, TexCoord.y);
	//color = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), interpolate);

	FragColor = vec4(objectColor * lightColor, 1.0f);
}