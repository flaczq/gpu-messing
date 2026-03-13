#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

uniform bool gizmoNegative;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	ourColor = aColor;
	vec3 gizmoPos = aPos;
	if (!gizmoNegative) {
		gizmoPos = max(vec3(0.0f), gizmoPos);
	}
	gl_Position = projection * view * model * vec4(gizmoPos, 1.0f);
}