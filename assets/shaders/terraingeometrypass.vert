#version 430 core
#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 vPos;

out VertOut {
	vec3 groundWorldPos;
	vec3 worldPos;
	float shellProgress;
} vertOut;

#include "_headeruniformbuffers.glsl"
#include "_headerterraininfo.glsl"

uniform float planeWorldWidth;

void main() {
	// Sample plane info
	vec2 flatPlanePos = vec2(chunkData.data[gl_InstanceID * 3 + 0], chunkData.data[gl_InstanceID * 3 + 1]);
	float shellProgress = chunkData.data[gl_InstanceID * 3 + 2];
	vertOut.shellProgress = shellProgress;

	vec4 worldPos = vec4(vPos.x * planeWorldWidth + flatPlanePos.x, 0, vPos.y * planeWorldWidth + flatPlanePos.y, 1);
	vec2 flatWorldPos = worldPos.xz;
	vec4 terrainInfo = getTerrainInfo(flatWorldPos, false);
	vec3 normal = normalize(vec3(-terrainInfo.y, 1, -terrainInfo.z));
	worldPos.y += terrainInfo.x;

	vertOut.groundWorldPos = worldPos.xyz;
	worldPos.xyz += normal * shellProgress * artisticParams.shellMaxHeight;

	vertOut.worldPos = worldPos.xyz;

	gl_Position = perFrameInfo.projectionMatrix * perFrameInfo.viewMatrix * worldPos;
}