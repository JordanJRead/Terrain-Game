#version 430 core

layout(location=0) in vec2 vPos;

out vec2 texCoord;

void main() {
	texCoord = (vPos + 1) / 2.0;
	gl_Position = vec4(vPos, 1, 1);
}