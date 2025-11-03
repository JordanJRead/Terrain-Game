#version 430 core

layout(location = 0) in vec3 vPos;

out vec3 outVPos;

#include "_headeruniformbuffers.glsl";

void main() {
	outVPos = vPos;
	gl_Position = (perFrameInfo.projectionMatrix * mat4(mat3(perFrameInfo.viewMatrix)) * vec4(vPos, 1)).xyww; // .xyww is to not mess with depth values
}