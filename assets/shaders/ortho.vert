#version 430 core

layout(location=0) in vec3 vPos;

#include "_headeruniformbuffers.glsl"

void main() {
	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * vec4(vPos, 1);
}