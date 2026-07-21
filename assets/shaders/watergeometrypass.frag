#version 430 core

in VertOut {
	vec3 worldPos;
} fragIn;

layout(location=0) out vec4 OutWaterWorldPos;
layout(location=1) out vec4 OutWaterNormal;

uniform vec3 planePos;

#include "_headeruniformbuffers.glsl"
#include "_headermath.glsl"
#include "_headerterraininfo.glsl"
#include "_headerwaterinfo.glsl"

void main() {
	vec2 flatWorldPos = fragIn.worldPos.xz;
	vec3 waterInfo = getWaterInfo(flatWorldPos);
	vec3 normal = normalize(vec3(-waterInfo.y, 1, -waterInfo.z));

	OutWaterWorldPos = vec4(fragIn.worldPos, 1);
	OutWaterNormal = vec4(normal, 1); // TODO add control for pixel-perfect normals?
}