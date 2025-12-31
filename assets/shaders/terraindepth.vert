#version 430 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 vPos;

#include "_headerterraininfo.glsl"
#include "_headeruniformbuffers.glsl"

// Per plane
uniform float planeWorldWidth;

void main() {
	// Sample plane info
	vec2 flatPlanePos = vec2(chunkData.data[gl_InstanceID * 3 + 0], chunkData.data[gl_InstanceID * 3 + 1]);
	float shellProgress = chunkData.data[gl_InstanceID * 3 + 2];

	vec4 worldPos = vec4(vPos.x * planeWorldWidth + flatPlanePos.x, 0, vPos.y * planeWorldWidth + flatPlanePos.y, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	worldPos.y += terrainInfo.x;
	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * worldPos;
}