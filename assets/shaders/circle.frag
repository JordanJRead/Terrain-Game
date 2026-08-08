#version 430 core

#include "_headeruniformbuffers.glsl"

in vec2 texCoord;
out vec4 FragColour;

uniform vec3 circleCenter;
uniform float radius;
uniform int spaceType;
uniform vec3 colour;
uniform float framebufferAspectRatio;

#define SPACE_WORLD 0
#define SPACE_VIEW 1
#define SPACE_NDC 2
#define SPACE_UV 3

void main() {
	vec4 clipCircleCenter;
	switch (spaceType) {
	case SPACE_WORLD:
		clipCircleCenter = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * vec4(circleCenter, 1);
		break;
	case SPACE_VIEW:
		clipCircleCenter = perFrameInfo.projectionMatrix * vec4(circleCenter, 1);
		break;
	case SPACE_NDC:
		clipCircleCenter = vec4(circleCenter, 1);
		break;
	case SPACE_UV:
		clipCircleCenter = vec4(circleCenter.xy * 2 - vec2(1), circleCenter.z, 1);
		break;
	}
	vec3 ndc = clipCircleCenter.xyz / clipCircleCenter.w;

	// Verify bounds
	if (ndc.x < -1 || ndc.y < -1 || ndc.x > 1 || ndc.y > 1 || ndc.z < -1 || ndc.z > 1) {
		discard;
	}

	vec2 uv = (ndc.xy + vec2(1)) / 2;
	if (length((texCoord - uv) * vec2(1, 1 / framebufferAspectRatio)) <= radius) {
		FragColour = vec4(colour, 1);
	}
	else {
		discard;
	}
}