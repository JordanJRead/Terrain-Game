#version 430 core

#include "_headeruniformbuffers.glsl"

layout(location=0) in float vIndex;
out float t;

#define SPACE_WORLD 0
#define SPACE_VIEW 1
#define SPACE_NDC 2
#define SPACE_UV 3

uniform vec3 linePos1;
uniform vec3 linePos2;

uniform int spaceType;

void main() {
	vec3 currentPos = vIndex == 0 ? linePos1 : linePos2;
	switch (spaceType) {
	case SPACE_WORLD:
		gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * vec4(currentPos, 1);
		break;
	case SPACE_VIEW:
		gl_Position = perFrameInfo.projectionMatrix * vec4(currentPos, 1);
		break;
	case SPACE_NDC:
		gl_Position = vec4(currentPos, 1);
		break;
	case SPACE_UV:
		gl_Position = vec4(currentPos.xy * 2 - vec2(1), currentPos.z, 1);
		break;
	}
	t = vIndex == 0 ? 0 : 1;
}