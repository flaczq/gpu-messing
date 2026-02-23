#version 330 core
out vec4 color;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float interpolate;

void main()
{
	vec2 flippedCoord = 4*vec2(1.0 - TexCoord.x, TexCoord.y);
	color = mix(texture(texture1, TexCoord), texture(texture2, flippedCoord), interpolate);
}