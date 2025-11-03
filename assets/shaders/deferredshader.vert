#version 430 core

layout(location=0) in vec2 vPos;
out vec2 texCoord;

void main() {
	texCoord = (vPos + vec2(1)) * 0.5;
	gl_Position = vec4(vPos, 0, 0);
}