#version 430 core

layout(location = 0) in vec3 vWorldPos;

out flat vec3 fragColor;

#include "_headeruniformbuffers.glsl"

uint rand(uint n) {
	uint state = n * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	word = (word >> 22u) ^ word;
	return word;
}

float randToFloat(uint n) {
	return float(n) / 4294967296.0;
}

void main() {
	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * vec4(vWorldPos, 1);
	fragColor = vec3(randToFloat(rand(gl_VertexID / 3)), randToFloat(rand(gl_VertexID / 3)), randToFloat(rand(gl_VertexID / 3)));
}