#version 430 core

in VertOut {
	vec3 worldPos;
} fragIn;

layout(location=0) out vec4 OutGroundWorldPosShellIndex;
layout(location=1) out vec4 OutWorldPosMountain;
layout(location=2) out vec4 OutNormalDoesTexelExist;

uniform vec3 planePos;

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"
#include "_headerterraininfo.glsl"
#include "_headerwaterinfo.glsl"

void main() {
	vec2 flatWorldPos = fragIn.worldPos.xz;
	vec3 waterInfo = getWaterInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));

	OutGroundWorldPosShellIndex = vec4(fragIn.worldPos, -2);
	OutWorldPosMountain = vec4(fragIn.worldPos, 0);
	OutNormalDoesTexelExist = vec4(normal, 0);
}