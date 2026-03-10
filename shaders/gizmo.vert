#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float gizmoLength;
uniform bool gizmoNegative;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	ourColor = aColor;
	vec3 gizmoPos = aPos * gizmoLength;
	if (gizmoNegative) {//FIXTHIS
		gizmoPos = abs(gizmoPos);
	}
	gl_Position = projection * view * model * vec4(gizmoPos, 1.0f);
}