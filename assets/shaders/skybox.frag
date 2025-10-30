#version 430 core

in vec3 outVPos;

out vec4 FragColor;

uniform samplerCube skybox;

#include "_headeruniformbuffers.glsl";

void main() {
	float d = dot(normalize(outVPos), perFrameInfo.dirToSun);
	d = clamp(d, 0.0, 1.0);
	vec3 sunAdd = pow(d, 200.0) * colours.sunColour;
	FragColor = texture(skybox, outVPos) + vec4(sunAdd, 1);
}